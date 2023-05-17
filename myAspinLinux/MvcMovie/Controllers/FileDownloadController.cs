using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System.IO;
using System.Threading.Tasks;
using StackExchange.Redis;
using System.Net.Http.Headers;
using System.Net;

[ApiController]
[Route("[controller]")]
public class FileDownloadController:ControllerBase{

     private readonly IConfiguration _config;
     public FileDownloadController(IConfiguration config)
     {
        this._config=config;
     }

   [HttpGet("files")]
    public async Task< IActionResult> GetFiles([FromServices] IConnectionMultiplexer connectionMultiplexer)
    {
       var redis = connectionMultiplexer.GetDatabase();
        var sortedSetKey = "uploadedFiles";
        var files = await redis.SortedSetRangeByRankAsync(sortedSetKey, 0, -1, Order.Descending);

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

    // [HttpGet("files/{fileName}")]
    // public IActionResult DownloadFile(string fileName)
    // {
    //     var filePath = Path.Combine("/path/to/files", fileName); // Replace with the actual file path on your Linux server

    //     if (!System.IO.File.Exists(filePath))
    //     {
    //         return NotFound();
    //     }

    //     var fileBytes = System.IO.File.ReadAllBytes(filePath);

    //     return File(fileBytes, "application/octet-stream", fileName);
    // }

    [HttpGet("files/{fileName}")]
    public IActionResult DownloadFile(string fileName)
    {
        string FILE_UPLOAD_PATH = _config["Position:Stroge"];

        var filePath=Path.Combine(FILE_UPLOAD_PATH,fileName);

        if (!System.IO.File.Exists(filePath))
        {
            return NotFound();
        }

        var fileInfo = new FileInfo(filePath);
        var totalLength = fileInfo.Length;

        var range = Request.Headers["Range"].ToString();
        var rangeParser = new RangeHeaderValue();

        var response = HttpContext.Response;
        if (RangeHeaderValue.TryParse(range, out rangeParser))
        {
            response.Headers.Add("Accept-Ranges", "bytes");

            long start, end;
            if (rangeParser.Ranges.Count == 1)
            {
                var rangeUnit = rangeParser.Ranges.First();
                start = rangeUnit.From ?? 0;
                end = rangeUnit.To ?? totalLength - 1;
                response.StatusCode = (int)HttpStatusCode.PartialContent;
            }
            else
            {
                start = 0;
                end = totalLength - 1;
                response.StatusCode = (int)HttpStatusCode.OK;
            }

            var length = end - start + 1;
            response.Headers.Add("Content-Length", length.ToString());
            response.Headers.Add("Content-Range", $"bytes {start}-{end}/{totalLength}");

            var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read, bufferSize: 4096, useAsync: true);
            fileStream.Seek(start, SeekOrigin.Begin);

            return new FileStreamResult(fileStream, "application/octet-stream");
        }
        else
        {
            var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read, bufferSize: 4096, useAsync: true);
            response.Headers.Add("Content-Length", totalLength.ToString());
            var file = File(fileStream, "application/octet-stream",enableRangeProcessing: true);
            return file;
        }
    }

        public IActionResult NotFound()
    {
        return Content("The source doesn't exist!","text/plain");
    }


}
