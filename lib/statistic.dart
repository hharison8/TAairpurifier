import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';

class Statistic extends StatefulWidget {
  const Statistic({super.key});

  @override
  State<Statistic> createState() => _StatisticState();
}

class _StatisticState extends State<Statistic> with TickerProviderStateMixin {
  late TabController _tabController;
  final _tabs = [
    const Tab(text: '1 Jam'),
    const Tab(text: '12 Jam'),
    const Tab(text: '24 Jam'),
  ];

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: 4, vsync: this);
    _tabController.index = 2;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          children: [
            const SizedBox(height: 20),
            Container(
              decoration: BoxDecoration(
                border: Border.all(width: 1),
                borderRadius: BorderRadius.circular(80),
              ),
              child: TabBar(
                controller: _tabController,
                tabs: _tabs,
                labelColor: Colors.white,
                labelStyle: const TextStyle(fontSize: 18),
                unselectedLabelColor: Colors.black,
                indicator: BoxDecoration(
                    color: Colors.white.withOpacity(0.2),
                    border: Border.all(width: 1),
                    borderRadius: BorderRadius.circular(80)),
              ),
            ),
            SizedBox(
              height: 20,
            ),
            Container(
              color: Colors.white,
              height: 400,
              width: 400,
              child: _LineChart(),
            )
          ],
        ),
      ),
    );
  }
}

class _LineChart extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return LineChart(sampleData1);
  }
}

LineChartData get sampleData1 => LineChartData(
      gridData: gridData,
      titlesData: titlesData,
      borderData: boarderData,
      lineBarsData: LineBarsData,
      minX: 0,
      maxX: 10,
      minY: 0,
      maxY: 5,
    );

List<LineChartBarData> get LineBarsData => [LineChartBarData1];

FlTitlesData get titlesData => FlTitlesData(
      bottomTitles: AxisTitles(
        sideTitles: bottomTitles,
      ),
      rightTitles: AxisTitles(
        sideTitles: SideTitles(showTitles: false),
      ),
      topTitles: AxisTitles(
        sideTitles: SideTitles(showTitles: false),
      ),
      leftTitles: AxisTitles(
        sideTitles: leftTitles,
      ),
    );

Widget leftTitlesWidget(double value, TitleMeta meta) {
  const style =
      TextStyle(fontSize: 15, fontWeight: FontWeight.bold, color: Colors.grey);
  String text;
  switch (value.toInt()) {
    case 1:
      text = '1m';
      break;
    case 2:
      text = '2m';
      break;
    case 3:
      text = '3m';
      break;
    case 4:
      text = '4m';
      break;
    case 5:
      text = '5m';
      break;
    default:
      return Container();
  }
  return Text(
    text,
    style: style,
    textAlign: TextAlign.center,
  );
}

SideTitles get leftTitles => SideTitles(
    getTitlesWidget: leftTitlesWidget,
    showTitles: true,
    interval: 1,
    reservedSize: 40);

Widget bottomTitlesWidget(double value, TitleMeta meta) {
  const style =
      TextStyle(fontSize: 15, fontWeight: FontWeight.bold, color: Colors.grey);
  Widget text;
  switch (value.toInt()) {
    case 2:
      text = const Text('2020', style: style);
      break;
    case 7:
      text = const Text('2021', style: style);
      break;
    case 12:
      text = const Text('2022', style: style);
      break;
    default:
      text = const Text('');
      break;
  }
  return SideTitleWidget(
    axisSide: meta.axisSide,
    space: 10,
    child: text,
  );
}

SideTitles get bottomTitles => SideTitles(
      showTitles: true,
      interval: 1,
      getTitlesWidget: bottomTitlesWidget,
    );

FlGridData get gridData => FlGridData(show: false);
FlBorderData get boarderData => FlBorderData(
    show: true,
    border: Border(
      bottom: BorderSide(color: Colors.grey, width: 4),
      left: const BorderSide(color: Colors.grey),
      right: const BorderSide(color: Colors.transparent),
      top: const BorderSide(color: Colors.transparent),
    ));

LineChartBarData get LineChartBarData1 => LineChartBarData(
      isCurved: true,
      color: Colors.purple,
      barWidth: 6,
      isStrokeCapRound: true,
      dotData: FlDotData(show: false),
      belowBarData: BarAreaData(show: false),
      spots: const [
        FlSpot(1, 1),
        FlSpot(3, 1.5),
        FlSpot(5, 1.6),
        FlSpot(7, 3.4),
        FlSpot(10, 2),
        FlSpot(12, 2.5),
        FlSpot(13, 1.6),
        FlSpot(1, 1),
      ],
    );
