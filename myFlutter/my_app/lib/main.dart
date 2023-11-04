import 'package:flutter/material.dart';
import 'package:my_app/config/app_colors.dart';
import 'package:my_app/login_page.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Desktop',
      theme: ThemeData(
        fontFamily: 'Urbanist',
        scaffoldBackgroundColor: AppColors.background,
        brightness: Brightness.dark,
        primarySwatch: Colors.blueGrey,
      ),
      home: const LoginPage(),
    );
  }
}
