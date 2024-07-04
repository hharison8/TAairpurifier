import 'package:flutter/widgets.dart';

import 'suhu.dart';

class SuhuData with ChangeNotifier {
  final List<DataSuhu> _chartSuhu = [];
  double _globalCurrentSensorValue = 0;

  List<DataSuhu> get chartSuhu => _chartSuhu;
  double get globalCurrentSensorValue => _globalCurrentSensorValue;

  void addDataSuhu(DataSuhu dataPoint) {
    _chartSuhu.add(dataPoint);
    notifyListeners();
  }

  void setGlobalCurrentSensorValue(double value) {
    _globalCurrentSensorValue = value;
    // Defer notifyListeners() call to avoid calling during build
    WidgetsBinding.instance!.addPostFrameCallback((_) {
      notifyListeners();
    });
  }

  // void _removeOldData() {
  //   final now = DateTime.now();
  //   _chartSuhu.removeWhere((data) => now.difference(data.time).inHours >= 3);
  // }
}
