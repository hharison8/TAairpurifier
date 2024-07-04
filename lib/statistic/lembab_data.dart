import 'package:flutter/widgets.dart';

import 'lembab.dart';

class LembabData with ChangeNotifier {
  final List<DataLembab> _chartLembab = [];
  double _globalCurrentSensorValue = 0;

  List<DataLembab> get chartLembab => _chartLembab;
  double get globalCurrentSensorValue => _globalCurrentSensorValue;

  void addDataLembab(DataLembab dataPoint) {
    _chartLembab.add(dataPoint);
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
  //   _chartLembab.removeWhere((data) => now.difference(data.time).inHours >= 3);
  // }
}
