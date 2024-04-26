import 'package:flutter/material.dart';
import 'package:flutter_application_1/login.dart';
import 'package:flutter_application_1/mainpage.dart';
import 'package:flutter_application_1/register.dart';
import 'package:flutter_application_1/statistic.dart';
import 'package:flutter_application_1/statistic/co.dart';
import 'package:flutter_application_1/statistic/lembab.dart';
import 'package:flutter_application_1/statistic/pm2.5.dart';
import 'package:flutter_application_1/statistic/suhu.dart';

class RouteGenerator {
  static Route<dynamic> generateRoute(RouteSettings settings) {
    // jika ingin mengirim argument
    // final args = settings.arguments;

    switch (settings.name) {
      case '/':
        return MaterialPageRoute(builder: (_) => const Login());
      case '/register':
        return MaterialPageRoute(builder: (_) => const Register());
      case '/mainpage':
        return MaterialPageRoute(builder: (_) => const mainpage());
      case '/statistic':
        return MaterialPageRoute(builder: (_) => const Statistic());
      case '/pm25':
        return MaterialPageRoute(builder: (_) => const PMPage());
      case '/co':
        return MaterialPageRoute(builder: (_) => const COPage());
      case '/suhu':
        return MaterialPageRoute(builder: (_) => const SuhuPage());
      case '/kelembaban':
        return MaterialPageRoute(builder: (_) => const LembabPage());
      default:
        return _errorRoute();
    }
  }

  static Route<dynamic> _errorRoute() {
    return MaterialPageRoute(builder: (_) {
      return Scaffold(
        appBar: AppBar(title: const Text("Error")),
        body: const Center(child: Text('Error page')),
      );
    });
  }
}
