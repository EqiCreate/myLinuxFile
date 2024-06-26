using Microsoft.AspNetCore.Server.Kestrel.Core;
using StackExchange.Redis;
using System.Configuration;
using System.Text;

internal class Program
{
    private static void Main(string[] args)
    {
        var builder = WebApplication.CreateBuilder(args);
        var localIp=GetLocalIPAddress();
        // Add services to the container.
        builder.Services.AddControllersWithViews();
        builder.Services.AddCors(options =>
        {
            options.AddDefaultPolicy(builder =>
            {
                builder.WithOrigins($"http://{localIp}:5002").AllowAnyHeader().AllowAnyMethod();
                builder.WithOrigins($"http://{localIp}:8080").AllowAnyHeader().AllowAnyMethod();
                builder.WithOrigins($"http://{localIp}:7268").AllowAnyHeader().AllowAnyMethod();
            });

        });
        // Add a singleton instance of ConnectionMultiplexer to the dependency injection container
        builder.Services.AddSingleton<IConnectionMultiplexer>(provider =>
          {
              // var configuration = ConfigurationOptions.Parse("");
              var configuration = ConfigurationOptions.Parse($"{localIp}");

              return ConnectionMultiplexer.Connect(configuration);
          });

        builder.Services.Configure<KestrelServerOptions>(option=>{
        option.AllowSynchronousIO=true;
         option.Limits.MaxRequestBodySize=1073741824;
         option.Limits.KeepAliveTimeout=TimeSpan.FromMinutes(15);
    });
        // builder.Services.Configure<PositionOptions>(option=>{
        //     builder.Configuration.GetSection(PositionOptions.Position);
        // });
        // Maintest();
// builder.Services.Configure<HttpClientHandler>(options=>{
//     // options.Proxy=null;i
//     // options.UseProxy=false;
//     options.Proxy=new WebProxy( new Uri("socks5://127.0.0.1:1090"),false);
//     options.UseProxy=true;
// });
// builder.WebHost .UseSockets(options=>{
// });
        var app = builder.Build();
        //   app.UseExceptionHandler("/Error/NotFound");
        // // app.UseStatusCodePagesWithReExecute("/Error/NotFound");
        // app.UseStatusCodePages(async context =>
        // {
        //     var statusCode = context.HttpContext.Response.StatusCode;
        //     if (statusCode == 404)
        //     {
        //         context.HttpContext.Response.Redirect("/Error/NotFound");
        //     }
        // });
        // app.UseExceptionHandler("/Error");
        // app.UseStatusCodePagesWithRedirects("/Error/NotFound/{0}");

        // Configure the HTTP request pipeline.
        if (!app.Environment.IsDevelopment())
        {
            Console.WriteLine("--------------------test IsDevelopment=false--------------------------------");
            // app.UseExceptionHandler("/Home/Error");
            // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
            app.UseHsts();
        }
        else{
            Console.WriteLine("--------------------test IsDevelopment=true--------------------------------");

        }

        app.UseHttpsRedirection();
        app.UseStaticFiles();

        app.UseRouting();

        app.UseAuthorization();
        app.UseCors();
        
        app.MapControllerRoute(
            name: "default",
            pattern: "{controller=Home}/{action=Index}/{id?}");
        // app.UseEndpoints(endpoints=>{
        //         endpoints.MapControllerRoute(
        //            name: "default",
        //              pattern: "{controller=Home}/{action=Index}/{id?}");

        //         // endpoints.MapControllerRoute(
        //         //     name: "NotFound",
        //         //     pattern: "*",
        //         //     defaults: new { controller = "Home", action = "NotFound" });
        // });
        //  app.Use(async (context, next) =>
        // {
        //     await next();
        //     if (context.Response.StatusCode == 404)
        //     {
        //         context.Request.Path = "/Error/NotFound";
        //         await next();
        //     }
        // });
    //     app.MapFallback(async (ctx) =>
    // {
    //     ctx.Response.Body.Write(Encoding.UTF8.GetBytes("404 from Fallback"));
    // });


        app.Run();
    }


    static async Task Maintest()
    {
        // var task = dotest();
        var task =new Lazy<Task<int>>(dotest);
        await Task.Delay(500);
        Console.WriteLine("Awaiting...");
        Console.WriteLine("Result is: " + await task.Value);
    }
    private static async Task<int> dotest()
    {
        Console.WriteLine("Async Start");
        await Task.Delay(800);
        Console.WriteLine("Async End");
        return 42;
    }
    public static string GetLocalIPAddress()
    {
        var host = Dns.GetHostEntry(Dns.GetHostName());
        foreach (var ip in host.AddressList)
        {
            if (ip.AddressFamily == AddressFamily.InterNetwork)
            {
                return ip.ToString();
            }
        }
        throw new Exception("No network adapters with an IPv4 address in the system!");
    }
}