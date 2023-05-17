using Microsoft.AspNetCore.Mvc;


public class ErrorController : Controller
{
    [Route("/Error/NotFound")]
    public IActionResult NotFound()
    {
        // Custom logic if needed
          return Content("~404~","text/plain");
    }
}
