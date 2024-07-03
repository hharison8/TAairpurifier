import 'package:flutter/foundation.dart';
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
    _removeOldData();
    notifyListeners();
  }

  void _removeOldData() {
    final now = DateTime.now();
    _chartSuhu.removeWhere((data) => now.difference(data.time).inHours >= 3);
  }
}
