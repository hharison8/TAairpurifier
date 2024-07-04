import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'co_data.dart';

class CO extends StatefulWidget {
  const CO({Key? key}) : super(key: key);

  @override
  _COState createState() => _COState();
}

class _COState extends State<CO> with AutomaticKeepAliveClientMixin {
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
      var provider = Provider.of<COData>(context, listen: false);
      provider
          .addDataCO(DataCO(DateTime.now(), provider.globalCurrentSensorValue));
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
          borderRadius: BorderRadius.all(Radius.circular(50)),
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
          var provider = Provider.of<COData>(context, listen: false);
          var documents = snapshot.data?.docs ?? [];
          for (var f in documents) {
            if (f.id == 'DHT11') {
              print('current CO = ${f['CO']}');
              provider.setGlobalCurrentSensorValue(double.parse(f['CO']));
            }
          }

          return Center(
            child: Consumer<COData>(
              builder: (context, chartCOProvider, child) {
                return SfCartesianChart(
                  primaryXAxis: DateTimeAxis(),
                  primaryYAxis: NumericAxis(
                      minimum: 0, maximum: 26, opposedPosition: true),
                  series: <ChartSeries>[
                    LineSeries<DataCO, DateTime>(
                      dataSource: chartCOProvider.chartCO,
                      xValueMapper: (DataCO data, _) => data.time,
                      yValueMapper: (DataCO data, _) => data.value,
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

class DataCO {
  final DateTime time;
  final double value;

  DataCO(this.time, this.value);
}
