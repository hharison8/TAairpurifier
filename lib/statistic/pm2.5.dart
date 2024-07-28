import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'pm2.5_data.dart';

class PM25 extends StatefulWidget {
  const PM25({Key? key}) : super(key: key);

  @override
  _PM25State createState() => _PM25State();
}

class _PM25State extends State<PM25> with AutomaticKeepAliveClientMixin {
  Timer? _timer;

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(const Duration(seconds: 1), _generateTrace);
  }

  _generateTrace(Timer t) {
    if (mounted) {
      var provider = Provider.of<PM25Data>(context, listen: false);
      provider
          .addDataPM(DataPM(DateTime.now(), provider.globalCurrentSensorValue));
    }
  }

  @override
  Widget build(BuildContext context) {
    super.build(context); // Ensure AutomaticKeepAliveClientMixin works
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
          var provider = Provider.of<PM25Data>(context, listen: false);
          var documents = snapshot.data?.docs ?? [];
          for (var f in documents) {
            if (f.id == 'DHT11') {
              print('current PM = ${f['PM25']}');
              provider.setGlobalCurrentSensorValue(double.parse(f['PM25']));
            }
          }

          return Center(
            child: Consumer<PM25Data>(
              builder: (context, chartPMProvider, child) {
                return SfCartesianChart(
                  primaryXAxis: DateTimeAxis(),
                  primaryYAxis: NumericAxis(
                      minimum: 0, maximum: 260, opposedPosition: true),
                  series: <ChartSeries>[
                    LineSeries<DataPM, DateTime>(
                      dataSource: chartPMProvider.chartPM,
                      xValueMapper: (DataPM data, _) => data.time,
                      yValueMapper: (DataPM data, _) => data.value,
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

class DataPM {
  final DateTime time;
  final double value;

  DataPM(this.time, this.value);
}
