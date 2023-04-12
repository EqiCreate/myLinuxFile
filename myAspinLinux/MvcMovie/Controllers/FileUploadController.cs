using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System.IO;
using System.Threading.Tasks;
using StackExchange.Redis;

[ApiController]
[Route("[controller]")]
public class FileUploadController : ControllerBase
{
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


        // Copy the file to the server
        using (var stream = new FileStream(filePath, FileMode.Create))
        {
            await file.CopyToAsync(stream);
        }
        // Store the metadata of the uploaded file in Redis
        var redis = connectionMultiplexer.GetDatabase();
        var hashKey = $"file:{uploadedFile.name.common}";
        var hashFields = new HashEntry[]
        {
            new HashEntry("filename", uploadedFile.name.common),
            new HashEntry("filepath", "media/"+uploadedFile.name.common)
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

}