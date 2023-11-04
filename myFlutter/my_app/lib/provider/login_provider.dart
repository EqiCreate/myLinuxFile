import 'package:flutter/cupertino.dart';

import '../data/response/login_response.dart';
import '../data/service/login_service.dart';

class LoginProvider extends ChangeNotifier {
  var username = '';
  var password = '';

  Future<LoginResponse> login() async {
    return LoginService(username, password).call();
  }
}
