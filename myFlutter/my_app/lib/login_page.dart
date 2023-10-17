import 'package:flutter/material.dart';
import 'config/app_strings.dart';

class LoginPage extends StatelessWidget {
  const LoginPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        body: SingleChildScrollView(
            child: SizedBox(
                height: MediaQuery.of(context).size.height,
                child: Padding(
                  padding: const EdgeInsets.all(24),
                  child: Column(children: const [
                    // Spacer(),
                    Text(AppStrings.helloWelcome),
                    SizedBox(
                      height: 16,
                    ),
                    Text("data2"),
                  ]),
                ))));
  }
}
