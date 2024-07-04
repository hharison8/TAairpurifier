import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:flutter/material.dart';
import 'package:flutter_application_1/statistic/suhu_data.dart';
import 'package:provider/provider.dart';
import 'package:syncfusion_flutter_charts/charts.dart';

class Suhu extends StatefulWidget {
  const Suhu({Key? key}) : super(key: key);

  @override
  _SuhuState createState() => _SuhuState();
}

class _SuhuState extends State<Suhu> with AutomaticKeepAliveClientMixin {
  Timer? _timer;

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(const Duration(seconds: 5), _generateTrace);
  }

  _generateTrace(Timer t) {
    if (mounted) {
      var provider = Provider.of<SuhuData>(context, listen: false);
      provider.addDataSuhu(
          DataSuhu(DateTime.now(), provider.globalCurrentSensorValue));
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
        margin: EdgeInsets.only(left: 15, right: 15, top: 15),
        padding: EdgeInsets.only(left: 10, right: 10, bottom: 10),
        decoration: const BoxDecoration(
            color: Colors.white,
            borderRadius: BorderRadius.all(Radius.circular(25))),
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
          var provider = Provider.of<SuhuData>(context, listen: false);
          var documents = snapshot.data?.docs ?? [];
          for (var f in documents) {
            if (f.id == 'DHT11') {
              print('current Suhu = ${f['Temperature']}');
              provider
                  .setGlobalCurrentSensorValue(double.parse(f['Temperature']));
            }
          }

          return Center(
            child: Consumer<SuhuData>(
              builder: (context, chartSuhuProvider, child) {
                return SfCartesianChart(
                  primaryXAxis: DateTimeAxis(),
                  primaryYAxis: NumericAxis(
                      minimum: 0, maximum: 50, opposedPosition: true),
                  series: <ChartSeries>[
                    LineSeries<DataSuhu, DateTime>(
                      dataSource: chartSuhuProvider.chartSuhu,
                      xValueMapper: (DataSuhu data, _) => data.time,
                      yValueMapper: (DataSuhu data, _) => data.value,
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

class DataSuhu {
  final DateTime time;
  final double value;

  DataSuhu(this.time, this.value);
}
