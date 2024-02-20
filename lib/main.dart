import 'package:flutter/material.dart';
import 'package:flutter_application_1/login.dart';
import 'package:flutter_application_1/mainpage.dart';
import 'package:flutter_application_1/register.dart';
import 'package:flutter_application_1/routes.dart';

Future main() async {
  WidgetsFlutterBinding.ensureInitialized();

  runApp(MaterialApp(
    debugShowCheckedModeBanner: false,
    initialRoute: 'login',
    routes: {
      'login': (context) => Login(),
      'register': (context) => Register(),
      'mainpage': (context) => mainpage(),
    },
    onGenerateRoute: RouteGenerator.generateRoute,
  ));
}

Future initialization(BuildContext? context) async {
  await Future.delayed(const Duration(seconds: 2));
}
