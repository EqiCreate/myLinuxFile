// using Microsoft.AspNetCore.Builder;
// using Microsoft.AspNetCore.Hosting;
// using Microsoft.AspNetCore.Http;
// using Microsoft.Extensions.Hosting;

// var builder = Host.CreateDefaultBuilder(args)
//     .ConfigureWebHostDefaults(builder =>
//     {
//         builder.UseUrls("http://192.168.3.61:5001");
//         builder.Configure(app =>
//         {
//             app.UseRouting();
//             app.UseEndpoints(endpoints =>
//             {
//                 endpoints.MapGet("/", async context =>
//                 {
//                     await context.Response.WriteAsync("Hello, World!");
//                 });
//             });
//         });
//     });

// await builder.Build().RunAsync();

using System.Net;

var builder = WebApplication.CreateBuilder(args);

var dd= builder.WebHost.UseUrls("http://192.168.3.61:5001");
// var dd= builder.WebHost.UseUrls("http://0.0.0.0:5001");

// var dd= builder.WebHost.UseUrls("http://localhost:5001");


// var builder = WebApplication.CreateDefaultBuilder(args);

// builder.us("http:192.168.3.61:5000");

// Add services to the container.
builder.Services.AddControllersWithViews();
// builder.Services.Configure<HttpClientHandler>(options=>{
//     // options.Proxy=null;
//     // options.UseProxy=false;
//     options.Proxy=new WebProxy( new Uri("socks5://127.0.0.1:1090"),false);
//     options.UseProxy=true;
// });
// builder.WebHost .UseSockets(options=>{
//       options.Listen(System.Net.IPAddress.Parse("192.168.3.61"), 5001); // 指定 IP 地址和端口号
// });
var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Home/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}
System.Console.WriteLine("--------------------test begin--------------------------------");

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseRouting();

app.UseAuthorization();

app.MapControllerRoute(
    name: "default",
    pattern: "{controller=Home}/{action=Index}/{id?}");

app.Run();
