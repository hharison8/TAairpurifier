import 'package:flutter/material.dart';

class COPage extends StatelessWidget {
  const COPage({super.key});

  @override
  Widget build(BuildContext context) => Scaffold(
        body: Container(
          color: const Color.fromRGBO(178, 209, 238, 1),
          child: Center(
            child: Text(
              'CO Content',
              style: TextStyle(
                  fontSize: 32,
                  color: Colors.white,
                  fontWeight: FontWeight.bold),
            ),
          ),
        ),
      );
}
