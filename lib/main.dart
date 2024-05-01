import 'dart:io';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:flutter_application_1/routes.dart';

Future main() async {
  WidgetsFlutterBinding.ensureInitialized();

  if (Platform.isAndroid) {
    await Firebase.initializeApp(
        options: const FirebaseOptions(
            apiKey: "AIzaSyCrLk-NMRaQgGH1GlYl0yxI5u4G-_NEjgY",
            appId: "1:245560806155:android:47831159404017c50a300e",
            messagingSenderId: "245560806155",
            projectId: "ta-air-purifier",
            databaseURL:
                "https://ta-air-purifier-default-rtdb.asia-southeast1.firebasedatabase.app",
            storageBucket: "ta-air-purifier.appspot.com"));
  } else {
    await Firebase.initializeApp();
  }

  runApp(const MaterialApp(
    debugShowCheckedModeBanner: false,
    onGenerateRoute: RouteGenerator.generateRoute,
  ));
}

Future initialization(BuildContext? context) async {
  await Future.delayed(const Duration(seconds: 2));
}
