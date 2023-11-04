import 'package:flutter/material.dart';
import 'package:my_app/config/app_icons.dart';
import 'package:my_app/config/app_strings.dart';
import 'package:flutter_svg/svg.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() {
    return _HomePageState();
  }
}

class _HomePageState extends State<HomePage> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
      title: const Text(AppStrings.appName),
      actions: [
        IconButton(
            onPressed: () {
              print("object");
            },
            icon: SvgPicture.asset(AppIcons.icLocation))
      ],
    ));
  }
}
