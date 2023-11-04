import 'package:flutter/material.dart';
import 'package:my_app/config/app_colors.dart';
import 'package:my_app/config/app_routes.dart';
import 'package:my_app/provider/login_provider.dart';
import 'package:provider/provider.dart';
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
          TextField(
            onChanged: (value) {
              context.read<LoginProvider>().username = value;
            },
            decoration: const InputDecoration(
                hintText: AppStrings.user,
                filled: true,
                border: OutlineInputBorder(
                    borderRadius: BorderRadius.all(Radius.circular(20)))),
          ),
          const SizedBox(
            height: 20,
          ),
          TextField(
            onChanged: (value) {
              context.read<LoginProvider>().password = value;
            },
            decoration: const InputDecoration(
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
              child: ElevatedButton(
                  onPressed: () {
                    Navigator.of(context).pushNamed(AppRoutes.home);
                  },
                  style: ElevatedButton.styleFrom(
                      backgroundColor: Colors.amber,
                      foregroundColor: Colors.white),
                  child: const Text(AppStrings.login))),
          const Spacer()
        ],
      ),
    )));
  }
}
