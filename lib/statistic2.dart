import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:flutter/material.dart';
import 'package:oscilloscope/oscilloscope.dart';

class Statistic2 extends StatefulWidget {
  @override
  _Statistic2State createState() => _Statistic2State();
}

class _Statistic2State extends State<Statistic2> {
  List<double> traceData = [];
  late Timer _timer;
  double globalCurrentSensorValue = 0;

  @override
  void dispose() {
    super.dispose();
    _timer.cancel();
  }

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(Duration(milliseconds: 500), _generateTrace);
  }

  _generateTrace(Timer t) {
    setState(() {
      traceData.add(globalCurrentSensorValue);
    });
  }

  Widget buildStreamBuilder() {
    Oscilloscope graph = Oscilloscope(
      showYAxis: true,
      yAxisColor: Colors.orange,
      padding: 20.0,
      backgroundColor: Colors.black,
      traceColor: Colors.green,
      yAxisMax: 500.0,
      yAxisMin: 0.0,
      dataSet: traceData,
    );

    return StreamBuilder(
      stream: FirebaseFirestore.instance.collection('EspData').snapshots(),
      builder: (context, AsyncSnapshot<QuerySnapshot> snapshot) {
        if (!snapshot.hasData) {
          return Center(
            child: CircularProgressIndicator(
              valueColor: AlwaysStoppedAnimation<Color>(Colors.red),
            ),
          );
        } else {
          var documents = snapshot.data?.docs ?? [];
          for (var f in documents) {
            if (f.id == 'DHT11') {
              print('current value = ${f['PM25']}');
              globalCurrentSensorValue = double.parse(f['PM25']);
            }
          }

          return Center(
            child: graph,
          );
        }
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('PM25'),
      ),
      body: buildStreamBuilder(),
    );
  }
}
