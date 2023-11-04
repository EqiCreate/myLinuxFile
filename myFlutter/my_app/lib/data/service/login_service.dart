import '../../config/app_config.dart';
import '../response/login_response.dart';
import 'base_service.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

class LoginService extends ServiceBase<LoginResponse> {
  final String username;
  final String password;

  LoginService(this.username, this.password);
  @override
  Future<LoginResponse> call() async {
    final result = await http.post(Uri.parse('${AppConfig.baseUrl}/login'),
        body: jsonEncode({
          'username': username,
          'password': password,
        }));
    return LoginResponse.fromJson(jsonDecode(result.body)['data']);
  }
}
