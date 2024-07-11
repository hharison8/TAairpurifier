import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'lembab_data.dart';

class Lembab extends StatefulWidget {
  const Lembab({Key? key}) : super(key: key);

  @override
  _LembabState createState() => _LembabState();
}

class _LembabState extends State<Lembab> with AutomaticKeepAliveClientMixin {
  Timer? _timer;

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(const Duration(minutes: 1), _generateTrace);
  }

  _generateTrace(Timer t) {
    if (mounted) {
      var provider = Provider.of<LembabData>(context, listen: false);
      provider.addDataLembab(
          DataLembab(DateTime.now(), provider.globalCurrentSensorValue));
    }
  }

  @override
  Widget build(BuildContext context) {
    super.build(context);
    return Scaffold(
      backgroundColor: const Color.fromRGBO(178, 209, 238, 1),
      body: Container(
        alignment: Alignment.topCenter,
        height: 590,
        margin: const EdgeInsets.only(left: 15, right: 15, top: 15),
        padding: const EdgeInsets.only(left: 10, right: 10, bottom: 10),
        decoration: const BoxDecoration(
          color: Colors.white,
          borderRadius: BorderRadius.all(Radius.circular(25)),
        ),
        child: buildStreamBuilder(),
      ),
    );
  }

  @override
  bool get wantKeepAlive => true;

  Widget buildStreamBuilder() {
    return StreamBuilder(
      stream: FirebaseFirestore.instance.collection('EspData').snapshots(),
      builder: (context, AsyncSnapshot<QuerySnapshot> snapshot) {
        if (!snapshot.hasData) {
          return const Center(
            child: CircularProgressIndicator(
              valueColor: AlwaysStoppedAnimation<Color>(Colors.red),
            ),
          );
        } else {
          var provider = Provider.of<LembabData>(context, listen: false);
          var documents = snapshot.data?.docs ?? [];
          for (var f in documents) {
            if (f.id == 'DHT11') {
              print('current Lembab = ${f['Humidity']}');
              provider.setGlobalCurrentSensorValue(double.parse(f['Humidity']));
            }
          }

          return Center(
            child: Consumer<LembabData>(
              builder: (context, chartLembabProvider, child) {
                return SfCartesianChart(
                  primaryXAxis: DateTimeAxis(),
                  primaryYAxis: NumericAxis(
                      minimum: 0, maximum: 100, opposedPosition: true),
                  series: <ChartSeries>[
                    LineSeries<DataLembab, DateTime>(
                      dataSource: chartLembabProvider.chartLembab,
                      xValueMapper: (DataLembab data, _) => data.time,
                      yValueMapper: (DataLembab data, _) => data.value,
                      width: 5,
                    ),
                  ],
                );
              },
            ),
          );
        }
      },
    );
  }
}

class DataLembab {
  final DateTime time;
  final double value;

  DataLembab(this.time, this.value);
}
