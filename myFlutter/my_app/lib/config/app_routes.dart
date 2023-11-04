import 'package:my_app/login_page.dart';
import 'package:provider/provider.dart';

import '../provider/login_provider.dart';

class AppRoutes {
  static final pages = {
    login: (context) => ChangeNotifierProvider(
          create: (context) => LoginProvider(),
          child: const LoginPage(),
        ),
    // home: (context) => HomePage(),
    // main: (context) => MainPage(),
    // editProfile: (context) => EditProfilePage(),
    // nearby: (context) => NearbyPage(),
    // user: (context) => UserPage(),
  };

  static const login = '/';
  static const home = '/home';
  static const main = '/main';
  static const editProfile = '/edit_profile';
  static const nearby = '/nearby';
  static const user = '/user';
}
