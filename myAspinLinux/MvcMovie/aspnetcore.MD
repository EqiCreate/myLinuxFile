### 20230517
- 下载的时候需要传送一个id或者key给服务器定位，如果只是name那么就会遍历搜索导致性能过低,但如何设计这唯一标识？
  - name 文件名，容易被猜
  - id，如果是incresed 可能会被猜到 -> random id
  - key ，前端需要进行 -> key
- 全局拦截notfound 会出现一个问题，比如/FileDownload 和/xx 会正常被拦截成为404界面，但是由于/FileDownload/files是正常的，伴随着的是/FileDownload/files/1 不会被拦截到???
- download 大文件
  - a 标签不安全!
  - 一般小文件直接读取所有字节到内存中返回就可以了
  - 不考虑断点续载的话，用异步的方式，将filestream 流按照固定大小块进行切分传送也可以了 如
    ```c#
         var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read, bufferSize: 4096, useAsync: true);
        return File(fileStream, "application/octet-stream", fileName);
    ```
   - 考虑到断点续传续载，文件大小超过2G的话，需要写文件响应头range+totallength,需本地文件寻址，需分块，异步
     - 响应头：```  response.Headers.Add("Content-Length", length.ToString());
        response.Headers.Add("Content-Range", $"bytes {start}-{end}/{totalLength}");```
    - 判断是否分块 /异步/寻址
    ```c#   
    if (RangeHeaderValue.TryParse(range, out rangeParser)){
        // 块的处理，以及定位续点的位置之类
             var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read, bufferSize: 4096, useAsync: true);
        fileStream.Seek(start, SeekOrigin.Begin);//寻址
    }
    else{
           var response = File(fileStream, "application/octet-stream");
        response.Headers.Add("Content-Length", totalLength.ToString());
    }
    ```

### 20230512
- pinvoke
  - DllImport(endpoint=xx,charset=xx) intptr,datastruct,destroy ...

### 20230502
- async scenerio  [here](./AsyncGuidance.md)
- 尽量用async/await 而不是直接返回task<T>,便于调试
- valuetask > task<T> > task run
- 从参数获取的对象可能会被更改因为对象变化了，netcore 提供了一种方式保持对象尽量不要改变，其实也是相当于复制一个出来并且自行析构
    ```c#
        [HttpGet("/fire-and-forget-3")]
    public IActionResult FireAndForget3([FromServices]IServiceScopeFactory serviceScopeFactory)
    {
        // This version of fire and forget adds some exception handling. We're also no longer capturing the PokemonDbContext from the incoming request.
        // Instead, we're injecting an IServiceScopeFactory (which is a singleton) in order to create a scope in the background work item.
        _ = Task.Run(async () =>
        {
            await Task.Delay(1000);

            // Create a scope for the lifetime of the background operation and resolve services from it
            using (var scope = serviceScopeFactory.CreateScope())
            {
                // This will a PokemonDbContext from the correct scope and the operation will succeed
                var context = scope.ServiceProvider.GetRequiredService<PokemonDbContext>();

                context.Pokemon.Add(new Pokemon());
                await context.SaveChangesAsync();
            }
        });

        return Accepted();
    }
    ```
- 懒加载 : lazy<Task<T>> 当且仅当await会去实例化task，比如用在conditionary<int,task<T>>直接new出来的话会实例化很多的task，用懒加载可以避免高昂的花销 conditionary<int,lazy<Task<T>>>
- "WindowsIdentity" 无法异步编程，总会有些系统调用无法使用!!!
  ```C#
     return WindowsIdentity.RunImpersonated(context =>
    {
         return _db.QueryAsync("SELECT Name from Products").Result;
    });
  ``` 
- 避免async void ，否则可能会造成进程崩溃
- await /async +syscontext 切换(continuewith(false)不携带context)
  - RunContinuationsAsynchronously ，强制标志该线程用当前的 线程而不是另外的线程，没有其他创建的花销
    - 常用于 cancellationtoken的包装
    ```c#
    var tcs=new TaskCompletionSource<object>(TaskContinuationAsynchronously);
    using(this.cancellationToken.Register((obj)=>{
        ((TaskCompletionSource<object>)obj).TrysetResult(null);//注册由this.cancellationToken 引发的事件

    },tcs))// 由当前线程引发
    var resultTask=await Task.WhenAny(other_task,tcs.task);// 看看是否有cancel事件发生
    if(resultTask=tcs.task){
        throw new OperationCanceledException(this.cancelationToken);
    }
    return await other_task;//执行计划的task
        ```
   - 定时取消线程
    ```c#
    public static async Task<T> TimeoutAfter<T>(this Task<T> task, TimeSpan timeout)
    {
        using (var cts = new CancellationTokenSource())
        {
            var delayTask = Task.Delay(timeout, cts.Token);

            var resultTask = await Task.WhenAny(task, delayTask);
            if (resultTask == delayTask)
            {
                // Operation cancelled
                throw new OperationCanceledException();
            }
            else
            {
                // Cancel the timer task so that it does not fire
                cts.Cancel();
            }

            return await task;
        }
    }
    ```
  - 文件操作
  ```c#
     using (var streamWriter = new StreamWriter(context.Response.Body))
    {
        await streamWriter.WriteAsync("Hello World");//可能会阻塞到缓冲区满
        await streamWriter.FlushAsync();//立马刷出来
    }
  ```