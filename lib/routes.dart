import 'package:flutter/material.dart';
import 'package:flutter_application_1/mainpage.dart';


class RouteGenerator {
  static Route<dynamic> generateRoute(RouteSettings settings) {
    // jika ingin mengirim argument
    // final args = settings.arguments; 

    switch (settings.name) {
      case '/':
        return MaterialPageRoute(builder: (_) => mainpage());
      default:
        return _errorRoute();
    }
  }

  static Route<dynamic> _errorRoute() {
    return MaterialPageRoute(builder: (_) {
      return Scaffold(
        appBar: AppBar(title: Text("Error")),
        body: Center(child: Text('Error page')),
      );
    });
  }
}