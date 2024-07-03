import 'package:flutter/foundation.dart';
import 'co.dart';

class COData with ChangeNotifier {
  final List<DataCO> _chartCO = [];
  double _globalCurrentSensorValue = 0;

  List<DataCO> get chartCO => _chartCO;
  double get globalCurrentSensorValue => _globalCurrentSensorValue;

  void addDataCO(DataCO dataPoint) {
    _chartCO.add(dataPoint);
    notifyListeners();
  }

  void setGlobalCurrentSensorValue(double value) {
    _globalCurrentSensorValue = value;
    _removeOldData();
    notifyListeners();
  }

  void _removeOldData() {
    final now = DateTime.now();
    _chartCO.removeWhere((data) => now.difference(data.time).inHours >= 3);
  }
}
