using StackExchange.Redis;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddControllersWithViews();
builder.Services.AddCors(options=>{
    options.AddDefaultPolicy(builder=>{
        builder.WithOrigins("http://localhost:5002").AllowAnyHeader().AllowAnyMethod();
        builder.WithOrigins("http://192.168.3.61:5001").AllowAnyHeader().AllowAnyMethod();
    });

});
// Add a singleton instance of ConnectionMultiplexer to the dependency injection container
  builder.Services.AddSingleton<IConnectionMultiplexer>(provider =>
    {
        // var configuration = ConfigurationOptions.Parse("192.168.3.61");
        var configuration = ConfigurationOptions.Parse("192.168.3.61");

        return ConnectionMultiplexer.Connect(configuration);
    });

// builder.Services.Configure<HttpClientHandler>(options=>{
//     // options.Proxy=null;
//     // options.UseProxy=false;
//     options.Proxy=new WebProxy( new Uri("socks5://127.0.0.1:1090"),false);
//     options.UseProxy=true;
// });
// builder.WebHost .UseSockets(options=>{
//       options.Listen(System.Net.IPAddress.Parse("192.168.3.61"), 5002); // 指定 IP 地址和端口号
// });
var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Home/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}
System.Console.WriteLine("--------------------test begin11--------------------------------");
// "applicationUrl": "http://192.168.3.61:7268/",
// "applicationUrl": "http://localhost:7269",

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseRouting();

app.UseAuthorization();

app.MapControllerRoute(
    name: "default",
    pattern: "{controller=Home}/{action=Index}/{id?}");
app.UseCors();
app.Run();
