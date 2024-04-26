import 'package:flutter/material.dart';

class PMPage extends StatelessWidget {
  const PMPage({super.key});

  @override
  Widget build(BuildContext context) => Scaffold(
        body: Container(
          color: const Color.fromRGBO(178, 209, 238, 1),
          child: Center(
            child: Text(
              'PM2.5 Content',
              style: TextStyle(
                  fontSize: 32,
                  color: Colors.white,
                  fontWeight: FontWeight.bold),
            ),
          ),
        ),
      );
}
