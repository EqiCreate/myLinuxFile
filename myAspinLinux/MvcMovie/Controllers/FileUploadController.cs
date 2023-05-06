using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System.IO;
using System.Threading.Tasks;
using StackExchange.Redis;

[ApiController]
[Route("[controller]")]
public class FileUploadController : ControllerBase
{

     private readonly IConfiguration _config;
     public FileUploadController(IConfiguration config)
     {
        this._config=config;
     }

    [HttpPost]
    public async Task<IActionResult> Upload(IFormFile file)
    {
        // Verify that a file was selected
        if (file == null || file.Length == 0)
            return BadRequest("No file was selected");
        var extension=Path.GetExtension(file.FileName);
        // Generate a unique file name to avoid overwriting existing files
        var fileName = Path.GetRandomFileName();
    //    var homePath= Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);//home

        // var filePath = Path.Combine(homePath,"mydownload" ,$"{fileName}{extension}");
       var mntPath= Environment.GetFolderPath(Environment.SpecialFolder.System);//home
        var filePath = Path.Combine("/media/michael","stroge","FILES" ,$"{fileName}{extension}");


        // Copy the file to the server
        using (var stream = new FileStream(filePath, FileMode.Create))
        {
            await file.CopyToAsync(stream);
        }

        return Ok(new { fileName });
    }

    [HttpPost("UploadwithRedis")]
    public async Task<IActionResult> UploadwithRedis(IFormFile file,[FromServices] IConnectionMultiplexer connectionMultiplexer)
    {
        if (file == null || file.Length == 0)
        return BadRequest("File not selected");

        var uploadedFile = new UploadedFile
        {
            name = new NameV{common= file.FileName}
        };
        var extension=Path.GetExtension(file.FileName);
        // Generate a unique file name to avoid overwriting existing files
        var fileName = Path.GetRandomFileName();
    //    var homePath= Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);//home

        // var filePath = Path.Combine(homePath,"mydownload" ,$"{fileName}{extension}");
       var mntPath= Environment.GetFolderPath(Environment.SpecialFolder.System);//home
        var filePath = Path.Combine("/media/michael","stroge","MEDIAS" ,$"{fileName}{extension}");

        try
        {
                // Copy the file to the server
            using (var stream = new FileStream(filePath, FileMode.Create))
            {
                await file.CopyToAsync(stream);
                if (stream.Length != file.Length)
                {
                    return StatusCode(500, "An error occurred while uploading the file.");
                }
            }
        }
        catch (System.Exception)
        {
             return StatusCode(500, "An error occurred while uploading the file.");
        }
      
  
        // Store the metadata of the uploaded file in Redis
        var redis = connectionMultiplexer.GetDatabase();
        var hashKey = $"file:{uploadedFile.name.common}";
        var hashFields = new HashEntry[]
        {
            new HashEntry("filename", uploadedFile.name.common),
            new HashEntry("filepath", "media/"+$"{fileName}{extension}")
        };
        await redis.HashSetAsync(hashKey, hashFields);
        await redis.SortedSetAddAsync("uploadedFiles", uploadedFile.name.common, DateTime.UtcNow.Ticks);

        return Ok(new { fileName });
    }
     

        [HttpGet("top10")]
        public async Task<IActionResult> GetTop10([FromServices] IConnectionMultiplexer connectionMultiplexer)
        {
            var redis = connectionMultiplexer.GetDatabase();
            var sortedSetKey = "uploadedFiles";
            var files = await redis.SortedSetRangeByRankAsync(sortedSetKey, 0, 9, Order.Descending);

            var result = new List<UploadedFile>();
            foreach (var file in files)
            {
                var hashKey = $"file:{file}";
                var hashFields = await redis.HashGetAllAsync(hashKey);

                result.Add(new UploadedFile
                {
                    name =new NameV{ common= hashFields.FirstOrDefault(x => x.Name == "filename").Value},
                    cca2=hashFields.FirstOrDefault(x => x.Name == "filepath").Value
                });
            }

            return Ok(result);
        }


    [HttpPost]
    [Route("multi-file")]
    public async Task<IActionResult> UploadFiles([FromServices] IConnectionMultiplexer connectionMultiplexer)
    {
        var filecollection = await Request.ReadFormAsync();
        var files=filecollection.Files;

        if (files == null || files.Count == 0)
        {
            return BadRequest("No files were selected for upload.");
        }

        var filePaths = new List<string>();
        try {
        foreach (var file in files) 
        {
              var extension=Path.GetExtension(file.FileName);
            var fileName = Path.GetRandomFileName();
            var filePath = Path.Combine("/media/michael","stroge","MEDIAS" ,$"{fileName}{extension}");
             var uploadedFile = new UploadedFile
                {
                    name = new NameV{common= file.FileName}
                };
            filePaths.Add(filePath);

           
                using (var stream = new FileStream(filePath, FileMode.Create))
                //  using (var stream = new MemoryStream())
                {
                    await file.CopyToAsync(stream);
                     if (stream.Length != file.Length)
                    {
                        return StatusCode(500, "An error occurred while uploading the file.");
                    }
                }

                 // Store the metadata of the uploaded file in Redis
                var redis = connectionMultiplexer.GetDatabase();
                var hashKey = $"file:{uploadedFile.name.common}";
                var hashFields = new HashEntry[]
                {
                    new HashEntry("filename", uploadedFile.name.common),
                    new HashEntry("filepath", "media/"+$"{fileName}{extension}")
                };
                await redis.HashSetAsync(hashKey, hashFields);
                await redis.SortedSetAddAsync("uploadedFiles", uploadedFile.name.common, DateTime.UtcNow.Ticks);
         }}
            catch (IOException ex)
            {
                // _logger.LogError(ex, $"Failed to copy {file.FileName} to {filePath}");
                return StatusCode(500, "An error occurred while uploading the file.");
            }
        return Ok(filePaths);

    }
    //   private const string FILE_UPLOAD_PATH = "/media/michael/stroge/MEDIAS/";
    [HttpPost("file-slice")]
    public async Task<IActionResult> UploadSlice([FromForm] IFormFile file, [FromForm]int chunkIndex, [FromForm]int totalChunks
    ,[FromServices] IConnectionMultiplexer connectionMultiplexer)
    {   
        string FILE_UPLOAD_PATH = _config["Position:Stroge"];
        if(string.IsNullOrWhiteSpace(FILE_UPLOAD_PATH) ){
            return StatusCode(500, "config error.");
        }
        int chunk_Index=Convert.ToInt32(chunkIndex);
        int total_Chunks=Convert.ToInt32(totalChunks);

         var fileName = Path.GetFileNameWithoutExtension(file.FileName);
         var extension = Path.GetExtension(file.FileName);
         var filePath = Path.Combine(FILE_UPLOAD_PATH, $"{fileName}_{chunkIndex}{extension}");
         try
         {
             using (var stream = new FileStream(filePath, FileMode.Create))
        {
            await file.CopyToAsync(stream);
        }
        if (chunk_Index == total_Chunks - 1) // Last chunk uploaded
            {
                var finalFilePath = Path.Combine(FILE_UPLOAD_PATH, $"{fileName}{extension}");
                 var uploadedFile = new UploadedFile
                {
                    name = new NameV{common= file.FileName}
                };
                using (var finalStream = new FileStream(finalFilePath, FileMode.Create))
                {
                    for (int i = 0; i < total_Chunks; i++)
                    {
                        var chunkFilePath = Path.Combine(FILE_UPLOAD_PATH, $"{fileName}_{i}{extension}");

                        using (var chunkStream = new FileStream(chunkFilePath, FileMode.Open))
                        {
                            await chunkStream.CopyToAsync(finalStream);
                        }

                        System.IO.File.Delete(chunkFilePath);
                    }
                }
                  // Store the metadata of the uploaded file in Redis
                var redis = connectionMultiplexer.GetDatabase();
                var hashKey = $"file:{uploadedFile.name.common}";
                var hashFields = new HashEntry[]
                {
                    new HashEntry("filename", uploadedFile.name.common),
                    new HashEntry("filepath", "media/"+$"{fileName}{extension}")
                };
                await redis.HashSetAsync(hashKey, hashFields);
                await redis.SortedSetAddAsync("uploadedFiles", uploadedFile.name.common, DateTime.UtcNow.Ticks);
            }

        return Ok();
         }
         catch (System.Exception e)
         {
            return StatusCode(500, "An error occurred while uploading the file.");
         }
       
    }
    // [HttpPost]
    // [Route("multi-file")]
    // public async Task<IActionResult> UploadFiles(List<IFormFile> files)
    // {
    //     // var files = Request.Form.Files;

    //     if (files == null || files.Count == 0)
    //     {
    //         return BadRequest("No files were selected for upload.");
    //     }

    //     var filePaths = new List<string>();

    //     foreach (var file in files)
    //     {
    //           var extension=Path.GetExtension(file.FileName);
    //         var fileName = Path.GetRandomFileName();
    //         var filePath = Path.Combine("/media/michael","stroge","MEDIAS" ,$"{fileName}{extension}");
    //         filePaths.Add(filePath);

    //          try
    //         {
    //             using (var stream = new FileStream(filePath, FileMode.Create))
    //             {
    //                 await file.CopyToAsync(stream);
    //                  if (stream.Length != file.Length)
    //                 {
    //                     return StatusCode(500, "An error occurred while uploading the file.");
    //                 }
    //             }
    //         }
    //         catch (IOException ex)
    //         {
    //             // _logger.LogError(ex, $"Failed to copy {file.FileName} to {filePath}");
    //             return StatusCode(500, "An error occurred while uploading the file.");
    //         }
    //     }

    //     return Ok(filePaths);
    // }

}
