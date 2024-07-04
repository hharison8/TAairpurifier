import 'package:flutter/widgets.dart';

import 'pm2.5.dart';

class PM25Data with ChangeNotifier {
  final List<DataPM> _chartPM = [];
  double _globalCurrentSensorValue = 0;

  List<DataPM> get chartPM => _chartPM;
  double get globalCurrentSensorValue => _globalCurrentSensorValue;

  void addDataPM(DataPM dataPoint) {
    _chartPM.add(dataPoint);
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
  //   _chartPM.removeWhere((data) => now.difference(data.time).inHours >= 3);
  // }
}
