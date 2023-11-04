import 'package:flutter/material.dart';
import 'package:my_app/config/app_colors.dart';
import 'config/app_strings.dart';

class LoginPage extends StatelessWidget {
  const LoginPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        body: SingleChildScrollView(
            child: SizedBox(
      height: MediaQuery.of(context).size.height,
      child: Column(
        children: [
          const Text(AppStrings.helloWelcome),
          const SizedBox(height: 20),
          const Spacer(),
          const Text("data"),
          const TextField(
            decoration: InputDecoration(
                hintText: AppStrings.user,
                filled: true,
                border: OutlineInputBorder(
                    borderRadius: BorderRadius.all(Radius.circular(20)))),
          ),
          const SizedBox(
            height: 20,
          ),
          const TextField(
            decoration: InputDecoration(
                hintText: AppStrings.helloWelcome,
                filled: true,
                border: OutlineInputBorder(
                    borderRadius: BorderRadius.all(Radius.circular(20)))),
          ),
          Align(
            alignment: Alignment.centerRight,
            child: TextButton(
              style: TextButton.styleFrom(foregroundColor: AppColors.white),
              onPressed: () => {},
              child: const Text(AppStrings.user),
            ),
          ),
          const SizedBox(
            height: 20,
          ),
          SizedBox(
            height: 48,
            child: Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: const [Text("data")],
            ),
          ),
          const Spacer()
        ],
      ),
    )));
  }
}
