import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';

class mainpage extends StatefulWidget {
  const mainpage({super.key});

  @override
  _MainpageState createState() => _MainpageState();
}

class _MainpageState extends State<mainpage> {




  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color.fromRGBO(178, 209, 238, 1),
      appBar: AppBar(
        iconTheme: const IconThemeData(color: Colors.black),
        backgroundColor: const Color.fromRGBO(255, 255, 255, 255),
        centerTitle: true,
        title: const Text(
          "Home", style: TextStyle(fontFamily: "Raleway"), //ganti logo
        ),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.start,
          children: <Widget>[
            Container(
              width: 280,
              height: 280,
              padding: const EdgeInsets.all(16),
              decoration: const BoxDecoration(
                color: Color.fromRGBO(160, 199, 235, 1),
                shape: BoxShape.circle,
                boxShadow: [
                  BoxShadow(
                      color: Color.fromRGBO(0, 0, 0, 0.4),
                      blurRadius: 2.0,
                      offset: Offset(0.0, 1.5)),
                ],
              ),
              child: Container(
                padding: const EdgeInsets.only(top: 64),
                decoration: const BoxDecoration(
                  color: Color.fromRGBO(255, 255, 255, 1),
                  shape: BoxShape.circle,
                ),
                child: const Column(
                  children: [
                    Text(
                      '50',
                      style: TextStyle(
                          color: Color.fromRGBO(160, 199, 235, 1),
                          fontWeight: FontWeight.bold,
                          fontSize: 100,
                          height: 0.95,
                          shadows: [
                            Shadow(
                              color: Colors.black12,
                              offset: Offset(-2, -2),
                              blurRadius: 5,
                            )
                          ]),
                    ),
                    Text(
                      'PM2.5',
                      style: TextStyle(
                          color: Color.fromRGBO(160, 199, 235, 1),
                          fontSize: 24),
                    )
                  ],
                ),
              ),
            ),
            Container(
              width: 400,
              height: 100,
              margin: const EdgeInsets.only(top: 16, bottom: 16),
              decoration: const BoxDecoration(
                color: Color.fromRGBO(160, 199, 235, 1),
                boxShadow: [
                  BoxShadow(
                    color: Color.fromRGBO(0, 0, 0, 0.4),
                    blurRadius: 2.0,
                    offset: Offset(0.0, 1.5),
                  ),
                ],
              ),
              child: Center(
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    StreamBuilder<DocumentSnapshot>(
                      stream: FirebaseFirestore.instance
                          .collection('EspData')
                          .doc('DHT11')
                          .snapshots(),
                      builder: (context, snapshot) {
                        if (!snapshot.hasData) {
                          return CircularProgressIndicator();
                        }

                        // Check if the document exists
                        if (snapshot.hasData && snapshot.data!.exists) {
                           // Access the data from the document
                           Map<String, dynamic>? data =
                             snapshot.data!.data() as Map<String, dynamic>?;

                        // Check if data is not null
                         if (data != null) {
                          // Access the specific fields
                          String coValue = data['CO'] ?? '0.0';

                        return Padding(
                          padding: const EdgeInsets.only(bottom: 3),
                          child: Text(
                            coValue,
                            style: TextStyle(
                              color: Colors.white,
                              fontWeight: FontWeight.bold,
                              fontSize: 40,
                              shadows: [
                                Shadow(
                                  color: Colors.black,
                                  blurRadius: 2,
                                  offset: Offset(0, 2),
                                ),
                              ],
                            ),
                          ),
                        );
                        } else {
                          // Handle the case when data is null
                          return Text('Data is null');
                        }
                        } else {
                          // Handle the case when the document doesn't exist
                          return Text('Document does not exist');
                        }
                      },
                    ),
                    Text(
                      'CO',
                      style: TextStyle(
                        color: Colors.white,
                        fontSize: 15,
                        shadows: [
                          Shadow(
                            color: Colors.black,
                            blurRadius: 2,
                            offset: Offset(0, 2),
                          ),
                        ],
                      ),
                    ),
                  ],
                ),
              ),
            ),
            SizedBox(
              width: 400,
              height: 156,
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Container(
                    width: 80,
                    height: 80,
                    padding: const EdgeInsets.only(left: 20),
                    decoration: const BoxDecoration(
                      color: Colors.white,
                      image:
                          DecorationImage(image: AssetImage('assets/Auto.png')),
                      shape: BoxShape.circle,
                      boxShadow: [
                        BoxShadow(
                            color: Color.fromRGBO(0, 0, 0, 0.4),
                            blurRadius: 2.0,
                            offset: Offset(0.0, 1.5)),
                      ],
                    ),
                  ),
                  Container(
                    width: 100,
                    height: 100,
                    margin: const EdgeInsets.only(top: 60),
                    decoration: const BoxDecoration(
                      color: Colors.white,
                      image: DecorationImage(
                          image: AssetImage('assets/Power.png')),
                      shape: BoxShape.circle,
                      boxShadow: [
                        BoxShadow(
                            color: Color.fromRGBO(0, 0, 0, 0.4),
                            blurRadius: 2.0,
                            offset: Offset(0.0, 1.5)),
                      ],
                    ),
                  ),
                  Container(
                    width: 80,
                    height: 80,
                    padding: const EdgeInsets.only(right: 20),
                    decoration: const BoxDecoration(
                      color: Colors.white,
                      image: DecorationImage(
                          image: AssetImage('assets/Manual.png')),
                      shape: BoxShape.circle,
                      boxShadow: [
                        BoxShadow(
                            color: Color.fromRGBO(0, 0, 0, 0.4),
                            blurRadius: 2.0,
                            offset: Offset(0.0, 1.5)),
                      ],
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(
              width: 400,
              height: 60,
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [SliderWidget()],
              ),
            ),
            SizedBox(
              width: 400,
              height: 80,
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  Container(
                    width: 120,
                    height: 60,
                    color: Colors.white,
                    child: Image.asset('assets/Stat.png'),
                  ),
                  Container(
                    width: 120,
                    height: 60,
                    color: Colors.white,
                    child: Image.asset('assets/Fan.png'),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class SliderWidget extends StatefulWidget {
  const SliderWidget({super.key});

  @override
  _SliderWidgetState createState() => _SliderWidgetState();
}

class _SliderWidgetState extends State<SliderWidget> {
  double _sliderValue = 0.0;

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Slider(
          value: _sliderValue,
          onChanged: (double value) {
            setState(() {
              _sliderValue = value;
            });
          },
          min: 0.0,
          max: 100.0,
          divisions: 100,
        ),
      ],
    );
  }
}
