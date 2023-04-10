using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System.IO;
using System.Threading.Tasks;

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
}
