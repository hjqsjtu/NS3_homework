/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sebastien Deronne <sebastien.deronne@gmail.com>
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/delay-jitter-estimation.h"
#include "ns3/basic-data-calculators.h"
#include "ns3/sqlite-data-output.h"

#include "wifi-example-apps.h"

// This example shows how to set Wi-Fi timing parameters through WifiMac attributes.
//
// Example: set slot time to 20 microseconds, while keeping other values as defined in the simulation script:
//
//          ./waf --run "wifi-timing-attributes --slot=20"
//
// Network topology:
//
//  Wifi 192.168.1.0
//
//       AP
//  *    *
//  |    |
//  n1   n2
//One node,two nodes,three nodes,
//different distance
//other parameters;

using namespace std;
using namespace ns3;

//This is the DelayJitterEstimation object ;
//its not be a ptr object ,so it can't be created by method like:Ptr<DelayJitterEstimation>
//there's  a question is how to use id.
//Today is 2019-04-17,I promise i will make effort to get success.
//That's a mistake,
NS_LOG_COMPONENT_DEFINE ("wifiParameterAndPerfomance");
/*
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
   在NS3中，所有时间类型，返回一个整数时，默认的单位是ns
*/
/*
   本程序统计了：吞吐量（Throughput)、丢包率、端到端时延(delay)、时延抖动(delayjitter)等主要网络性能参数
   通过改变网络参数，多次运行程序统计上述性能参数的平均值
        （1）节点距离：distance
        （2）時隙：slot(默认值：9us)
        （3）短時隙：sifs（默认值：16us）
        （4）协议类型：默认值WIFI_PHY_STANDARD_80211n_2_4GHZ
        （5）后续可能增加其他参数的测试
*/
//对应于发送端，发送一个包增加计数1
void TxCallback (Ptr<CounterCalculator<uint32_t> > datac,
                 std::string path, Ptr<const Packet> packet) {
  NS_LOG_INFO ("Sent frame counted in " <<
               datac->GetKey ());
  datac->Update ();//增加计数值datac的值1
  // end TxCallback
}

int main (int argc, char *argv[])
{
  double distance = 30.0;
  string format ("db");

  string experiment ("wifi-parameters-performance");//实验名称
  string strategy ("wifi-default");//WIFI的策略
  string specID;//实验时节点的距离
  string runID;//根据时间生成的一个运行编号

  {
    stringstream sstr;
    sstr << "run-" << time (NULL);
    runID = sstr.str ();
  }


  uint32_t slot = 9; //slot time in microseconds
  uint32_t sifs = 16; //SIFS duration in microseconds
  //应答超时时间（ACK timeout),反复测试，只有大于这个数值，才能正常收发
  uint32_t ackTimeout = 400; //ACK timeout duration in microseconds
  //clear to send timeout
  uint32_t ctsTimeout = 75; //CTS timeout duration in microseconds
  uint32_t rifs = 2; //RIFS duration in microseconds
  uint32_t basicBlockAckTimeout = 281; //Basic Block ACK timeout duration in microseconds
  uint32_t compressedBlockAckTimeout = 107; //Compressed Block ACK timeout duration in microseconds
  double simulationTime =10; //simulation time in seconds
  uint32_t difs = 60 ;//DIFS (data frame time)
  uint32_t maxPackets = 1000;
  uint32_t verbose=0;

  uint32_t nodes=2;

  //使能所有Log信息
  /*
  在涉及到大量的通信数据发送测试时，最好关闭Log信息的屏幕显示，否则影响速度，在调试时可以使用
  在涉及到大量的通信数据发送测试时，最好关闭Log信息的屏幕显示，否则影响速度，在调试时可以使用
  在涉及到大量的通信数据发送测试时，最好关闭Log信息的屏幕显示，否则影响速度，在调试时可以使用
  在涉及到大量的通信数据发送测试时，最好关闭Log信息的屏幕显示，否则影响速度，在调试时可以使用
  在涉及到大量的通信数据发送测试时，最好关闭Log信息的屏幕显示，否则影响速度，在调试时可以使用
  在涉及到大量的通信数据发送测试时，最好关闭Log信息的屏幕显示，否则影响速度，在调试时可以使用
    */
  //LogComponentEnable ("wifiParameterAndPerfomance", LOG_LEVEL_ALL);
  //LogComponentEnable("Wifi-example-apps",LOG_LEVEL_ALL);

  ////以下添加到命令行的参数，可以在运行程序时，修改具体的值，如下
  ////假设当前目录在ns-allinone-3.29/ns-3.29/examples/scratch目录下
  ////sudo ../../war --run "wifi-time-attributes --distance=100"
  ////sudo ../../war --run "wifi-time-attributes --maxPackets=2000 --simulationTime=10"
  ////这样的好处是可以不用每次参数变化都修改程序，提高效率
  CommandLine cmd;

  ////先面这些命令行参数控制实验的主要运行参数
  cmd.AddValue ("distance", "Distance apart to place nodes (in meters).",distance);
  cmd.AddValue ("format", "Format to use for data output.",format);
  cmd.AddValue ("experiment", "Identifier for experiment.",experiment);
  cmd.AddValue ("strategy", "Identifier for strategy.",strategy);
  cmd.AddValue ("run", "Identifier for run.",runID);
  cmd.AddValue("nodes","总节点数",nodes);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue("maxPackets","total packets to send/receive",maxPackets);
  cmd.AddValue("verbose","Display detail information or not",verbose);
  //下面是Wifi网络的相关主要参数的解释及其默认值，
  //本程序的目的就是检验各主要参数对wifi性能的影响
  //RTS:Request to send(receiver to use)
  //CTS:clear to send(sender to use)
  //SIFS：最小的帧间间隔
  //DIFS：DIFS = SIFS + (2 * Slot time)，DCF协议帧间隔
  //PIFS = SIFS + Slot time，AP的帧间隔
  //EIFS = 发送错误之后（如没有收到ACK）需要等待的时间
  //在前一帧出错的情况下，发送节点不得不延迟EIFS而不是DIFS时间段后再发送下一帧。
  //RIFS：缩短帧间隙
  cmd.AddValue ("slot", "Slot time in microseconds", slot);
  cmd.AddValue ("sifs", "SIFS duration in microseconds", sifs);
  cmd.AddValue ("difs", "DIFS duration in microseconds", difs);
  cmd.AddValue ("ackTimeout", "ACK timeout duration in microseconds", ackTimeout);
  cmd.AddValue ("ctsTimeout", "CTS timeout duration in microseconds", ctsTimeout);
  cmd.AddValue ("rifs", "RIFS duration in microseconds", rifs);
  cmd.AddValue ("basicBlockAckTimeout", "Basic Block ACK timeout duration in microseconds", basicBlockAckTimeout);
  cmd.AddValue ("compressedBlockAckTimeout", "Compressed Block ACK timeout duration in microseconds", compressedBlockAckTimeout);
  //上面是Wifi网络的相关主要参数的解释及其默认值，
  //本程序的目的就是检验各主要参数对wifi性能的影响

  //使上面的所有参数起作用，具体实现的是从用户输入的命令行中获取各个参数的值，并赋值给对应的变量
  cmd.Parse (argc,argv);
  if(verbose)
  {
      std::cout << "以下是本次实验的主要参数配置" << std::endl;
      std::cout << "以下列格式运行程序可以改变实际运行参数值，多个参数可以并列分别用 --参数名(无空格)=(无空格)参数值  对加以修改." << std::endl;
      std::cout << "下面的实例，distance=100 slot=12." << std::endl;
      std::cout << "sudo ../../waf --run \"wifi-timing-attributes --distance=100 --slot=12 \" " << std::endl;
      std::cout << "当前所有可配置参数如下所示" << std::endl;

      std::cout << "distance = " << distance << "m"  << std::endl;
      std::cout << "format = " << format << std::endl;
      std::cout << "experiment = " << experiment << std::endl;
      std::cout << "strategy = " << strategy << std::endl;
      std::cout << "run = " << runID << std::endl;
      std::cout << "nodes = " << nodes << std::endl;
      std::cout << "simulationTime = " << simulationTime << "s" << std::endl;
      std::cout << "maxPackets = " << maxPackets << std::endl;

      std::cout << "slot = " << slot << "us"  << std::endl;
      std::cout << "sifs = " << sifs << "us"  << std::endl;
      std::cout << "difs = " << difs << "us"  << std::endl;
      std::cout << "ackTimeout = " << ackTimeout << "us"  << std::endl;
      std::cout << "ctstimeout = " << ctsTimeout << "us"  << std::endl;
      std::cout << "rifs = " << rifs << "us"  << std::endl;
      std::cout << "basicBlockAckTimeout = " << basicBlockAckTimeout << "us"  << std::endl;
      std::cout << "compressedBlockAckTimeout = " << compressedBlockAckTimeout << "us"  << std::endl;
      std::cout << "参数输出结束" << std::endl;
  }


  ////以下是参数判断（是否合法）及转换处理
  //判断监测数据的输出格式，目前只支持omnet(文本）和db(sqlite3数据库）2种格式
  if (format != "omnet" && format != "db") {
      NS_LOG_ERROR ("Unknown output format '" << format << "'");
      return -1;
    }

  #ifndef STATS_HAS_SQLITE3
  if (format == "db") {
      NS_LOG_ERROR ("sqlite support not compiled in.");
      return -1;
    }
  #endif

  {
    stringstream sstr ("");
    sstr << distance;
    ////sstr << slot;
    specID = sstr.str ();  //将节点间距离和节点数转换成本次配置的字符串ID
  }
  ////以上是参数判断（是否合法）及转换处理


  ////以下是wifi网络定义：包括拓扑、各层协议等
  ////以下是wifi网络定义：包括拓扑、各层协议等
  ////以下是wifi网络定义：包括拓扑、各层协议等
  ////以下是wifi网络定义：包括拓扑、各层协议等

  //参考损失模型为对数距离传播损失模型，参考增益为40.046dB
  Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue (40.046));

  NS_LOG_INFO ("Creating nodes.");

  //创建节点，在程序中节点创建的顺序，决定了其在NodeList中的顺序，序号从0开始
  NodeContainer wifiStaNode;
  //生成2个Wifi站节点
  wifiStaNode.Create (nodes);//2个WIFI节点，分别是：NodeList/0和NodeList/1

  //生成1个Wifi AP节点（Access Point，访问点）
  NodeContainer wifiApNode;
  wifiApNode.Create (1);//1个WIFI AP节点，是NodeList/2，是顺序分配的

  //物理层采用YansWifi模型
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  //这里的协议选择用802.11n (2.4 GHz)
  WifiHelper wifi;
  ////wifi.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
  wifi.SetStandard(WIFI_PHY_STANDARD_80211a);

  //MCS:Modulation and Code scheme:调制和编码策略
  //DataMode:发送数据包的模式：High Throughput MultiCaSt 7
  //ControlMode:发送RTS包的模式：High Throughput MultiCaSt 0
  //这些模式用于描述物理层的模式，在wifi-phy.cc中有定义
  //对WIFI_PHY_STANDARD_80211n_2_4GHZ协议来说，下面的码率控制器必须定义，并指明其数据包和RTS包的模式
  //wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  //                             "DataMode", StringValue ("HtMcs7"),
  //                             "ControlMode", StringValue ("HtMcs0"));
  WifiMacHelper mac;

  //WIFI 有2种协调功能：分布式协调功能DCF和点协调功能PCF
  //Install PHY and MAC
  Ssid ssid = Ssid ("ns3-wifi");
  //自定义的Wifi节点类型串和Ssid
  //第一个参数是物理层类名称，比如:StaWifiMac是描述Wifi普通节点访问控制层的类名;
  //包含：RIFS，ackTimeout，ctsTimeout,SIFS,slot，PIF，EIFS，DIFS，blockAckTimeout，
  //compressBlockAckTimeout等参数
  mac.SetType ("ns3::AdhocWifiMac","Ssid", SsidValue (ssid));
  ////mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid));

  NetDeviceContainer staDevice;
  //将Wifi某种协议安装到wifi节点，当前实例中是802.11n 2.4GHz
  //安装Wifi协议，产生对应的设备（网卡），3个参数：物理层、访问控制层（MAC）、对应的Wifi节点
  staDevice = wifi.Install (phy, mac, wifiStaNode);



  //自定义的AP类型串和SSID
  //Wifi的AP的名字串以及SSid与Wifi的普通节点不同
  //ApWifiMac这个名称是唯一的，对应Wifi AP节点的Mac层类
  //具体可控制参数参考StaWifiMac
  mac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));

  NetDeviceContainer apDevice;
  //将Wifi某种协议安装到wifi的AP节点，当前实例中是802.11n 2.4GHz
  apDevice = wifi.Install (phy, mac, wifiApNode);


  //Once install is done, we overwrite the standard timing values
  //设置所有Wifi访问控制层的主要参数，Mac主要参数
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/Sifs", TimeValue (MicroSeconds (sifs)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/EifsNoDifs", TimeValue (MicroSeconds (difs)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/CtsTimeout", TimeValue (MicroSeconds (ctsTimeout)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/Rifs", TimeValue (MicroSeconds (rifs)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/BasicBlockAckTimeout", TimeValue (MicroSeconds (basicBlockAckTimeout)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/CompressedBlockAckTimeout", TimeValue (MicroSeconds (compressedBlockAckTimeout)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/Slot", TimeValue (MicroSeconds (slot)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/AckTimeout", TimeValue (MicroSeconds (ackTimeout)));


  //节点移动模型
  //节点移动模型
  //节点移动模型
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  //2个节点的距离是固定的，采用ListPositionAllocator形式指定
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (10.0, 0.0, 0.0));
  positionAlloc->Add (Vector (0.0, distance, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  //mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNode);
  //节点移动模型
  //节点移动模型
  //节点移动模型

  //Internet stack
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNode);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staNodeInterface;
  Ipv4InterfaceContainer apNodeInterface;

  //IP地址从192.168.1.1开始分配
  //这里应该是分配2个IP地址，分别是192.168.1.1和192.168.1.2
  staNodeInterface = address.Assign (staDevice);
  //这里分配一个IP地址，是：192.168.1.3
  apNodeInterface = address.Assign (apDevice);
  ////如果还有其他节点则顺序分配IP地址

  ////以上是wifi网络定义：包括拓扑、各层协议等
  ////以上是wifi网络定义：包括拓扑、各层协议等
  ////以上是wifi网络定义：包括拓扑、各层协议等


  //Setting applications
  //uint32_t port = 9;
  Ptr<Node> appSource = NodeList::GetNode (0);
  Ptr<Sender> sender = CreateObject<Sender>();
  appSource->AddApplication (sender);

  sender->SetAttribute("Destination",Ipv4AddressValue(staNodeInterface.GetAddress(1)));
  sender->SetAttribute("NumPackets",UintegerValue(maxPackets));
  sender->SetAttribute("PacketSize",UintegerValue(1472));
  //sender->SetAttribute("Port",UintegerValue(port));
  //sender->SetAttribute("Interval",StringValue ("ns3::ConstantRandomVariable[Constant=0.00002]"));
  sender->SetAttribute("Interval",DoubleValue(0.0002));
  sender->SetStartTime (Seconds (1));
  //sender->SetStopTime(Seconds (simulationTime + 1));

  //Config::Set ("/NodeList/*/ApplicationList/*/$Sender/Destination",
  //             Ipv4AddressValue ("192.168.1.2"));

  Ptr<Node> appSink = NodeList::GetNode (1);
  Ptr<Receiver> receiver = CreateObject<Receiver>();
  appSink->AddApplication (receiver);
  //receiver->SetAttribute("Port",UintegerValue(port));
  receiver->SetStartTime (Seconds (0));


  //Populate routing table
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /*
     DataCollector是一个数据收集的类，其主要的表有3个：
        （1）Experiments，描述某次实验的整体信息（运行编号是唯一索引）
        （2）MetaData，包含作者信息等
        （3）Singletons，具体的数据收集表，通过调用AddDataCalculator添加要收集的数据，主要类型有：
            （a）CounterCalculator，计数器，例如用于数据包的计数
            （b）PacketSizeMinMaxAvgTotalCalculator，统计包大小
            （c）TimeMinMaxAvgTotalCalculator，时间相关，比如延迟，其中包括计数值、最大、最小、平均、总值等
            （d）MinMaxAvgTotalCalculator，某数据的统计，包括：计数值、最大、最小、总值
            上述每个DataCollector会把所有数据每个一行写入Singletons表中，
            run列-------对应Experiments中的某次实验编号
            name列------对应SetContext设置的每个DataCollector的名称
            variable列--对应如计数值、最大、最小值等，前面加上一个Key，比如：jitter-count，jitter-max等
            value列-----具体的值
    DataCollector的数据可以写入omnet文本文件，也可以写入sqlite数据库中，本程序选择写入sqlite3数据库中，便于数据交换
  */
  DataCollector data;
  ////对应Experiments表，实验整体描述
  data.DescribeRun (experiment,
                    strategy,
                    specID,
                    runID);

  ////对应MetaData表，可以写入一些元数据，本程序写入作者信息
  data.AddMetadata ("author", "HJQ-sjtu");


  //下面是加入的多个数据收集器实例，分别收集不同的数据，用于后期分析统计，
  //主要是统计：吞吐量（Throughput)、丢包率（PacketLoss）、端到锻延迟（delay)、延迟抖动(delayjitter)

  //发送帧计数
  //发送帧计数
  //发送帧计数
  Ptr<CounterCalculator<uint32_t> > totalTx =
    CreateObject<CounterCalculator<uint32_t> >();
  totalTx->SetKey ("wifi-tx-frames");
  totalTx->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                   MakeBoundCallback (&TxCallback, totalTx));
  data.AddDataCalculator (totalTx);
  //发送帧计数
  //发送帧计数
  //发送帧计数


  //接收帧计数
  //接收帧计数
  //接收帧计数
  Ptr<PacketCounterCalculator> totalRx =
    CreateObject<PacketCounterCalculator>();
  totalRx->SetKey ("wifi-rx-frames");
  totalRx->SetContext ("node[1]");
  Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                   MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                 totalRx));
  data.AddDataCalculator (totalRx);
  //接收帧计数
  //接收帧计数
  //接收帧计数


  //发送包计数
  //发送包计数
  //发送包计数
  Ptr<PacketCounterCalculator> appTx =
    CreateObject<PacketCounterCalculator>();
  appTx->SetKey ("sender-tx-packets");
  appTx->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/ApplicationList/*/$Sender/Tx",
                   MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                 appTx));
  data.AddDataCalculator (appTx);
  //发送包计数
  //发送包计数
  //发送包计数

  //接收包计数
  //接收包计数
  //接收包计数
  Ptr<CounterCalculator<> > appRx =
    CreateObject<CounterCalculator<> >();
  appRx->SetKey ("receiver-rx-packets");
  appRx->SetContext ("node[1]");
  receiver->SetCounter (appRx);
  data.AddDataCalculator (appRx);
  //接收包计数
  //接收包计数
  //接收包计数


  //包大小统计数据
  //包大小统计数据
  //包大小统计数据
  Ptr<PacketSizeMinMaxAvgTotalCalculator> appTxPkts =
    CreateObject<PacketSizeMinMaxAvgTotalCalculator>();
  appTxPkts->SetKey ("tx-pkt-size");
  appTxPkts->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/ApplicationList/*/$Sender/Tx",
                   MakeCallback
                     (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate,
                     appTxPkts));
  data.AddDataCalculator (appTxPkts);
  //包大小统计数据
  //包大小统计数据
  //包大小统计数据


  //端到锻延迟统计
  //端到锻延迟统计
  //端到锻延迟统计
  Ptr<TimeMinMaxAvgTotalCalculator> delayStat =
    CreateObject<TimeMinMaxAvgTotalCalculator>();
  delayStat->SetKey ("delay");
  delayStat->SetContext ("delay-context");
  receiver->SetDelayTracker (delayStat);
  data.AddDataCalculator (delayStat);
  //端到锻延迟统计
  //端到锻延迟统计
  //端到锻延迟统计

  //延迟抖动相关数据
  //延迟抖动相关数据
  //延迟抖动相关数据
  Ptr<MinMaxAvgTotalCalculator<double>> jitterStat =
    CreateObject<MinMaxAvgTotalCalculator<double>>();
  jitterStat->SetKey ("jitter");
  jitterStat->SetContext ("jitter-context");
  receiver->SetJitterTracker(jitterStat);
  data.AddDataCalculator (jitterStat);
  //延迟抖动相关数据
  //延迟抖动相关数据
  //延迟抖动相关数据

  //数据发送用时统计，与收发包相关数据一起，可以进行吞吐量统计：吞吐量=[包数×包大小（字节数）×8]/[发送数据用时*1000000]，单位：Mbit/s
  //数据发送用时统计，与收发包相关数据一起，可以进行吞吐量统计：吞吐量=[包数×包大小（字节数）×8]/[发送数据用时*1000000]，单位：Mbit/s
  //数据发送用时统计，与收发包相关数据一起，可以进行吞吐量统计：吞吐量=[包数×包大小（字节数）×8]/[发送数据用时*1000000]，单位：Mbit/s
  Ptr<TimeMinMaxAvgTotalCalculator> lastStat =
    CreateObject<TimeMinMaxAvgTotalCalculator>();
  lastStat->SetKey ("last");
  lastStat->SetContext ("last-context");
  sender->SetLastTracker (lastStat);
  data.AddDataCalculator (lastStat);
  //数据发送用时统计，与收发包相关数据一起，可以进行吞吐量统计：吞吐量=[包数×包大小（字节数）×8]/[发送数据用时*1000000]，单位：Mbit/s
  //数据发送用时统计，与收发包相关数据一起，可以进行吞吐量统计：吞吐量=[包数×包大小（字节数）×8]/[发送数据用时*1000000]，单位：Mbit/s
  //数据发送用时统计，与收发包相关数据一起，可以进行吞吐量统计：吞吐量=[包数×包大小（字节数）×8]/[发送数据用时*1000000]，单位：Mbit/s

  //------------------------------------------------------------
  //-- Run the simulation
  //--------------------------------------------
  NS_LOG_INFO ("Run Simulation.");
  //Set simulation time and launch simulation
  Simulator::Stop (Seconds (simulationTime + 1));

  ////以下是NetAnim相关，用于生成网络拓扑图，写文档时可以用
  ////以下是NetAnim相关，用于生成网络拓扑图，写文档时可以用
  ////以下是NetAnim相关，用于生成网络拓扑图，写文档时可以用
  //AnimationInterface anim("scratch/attributes.xml");
  //anim.SetMaxPktsPerTraceFile(1000000u);
  ////以下是NetAnim相关，用于生成网络拓扑图，写文档时可以用
  ////以下是NetAnim相关，用于生成网络拓扑图，写文档时可以用
  ////以下是NetAnim相关，用于生成网络拓扑图，写文档时可以用

  //以下语句用于统计仿真总用时，
  //以下语句用于统计仿真总用时，
  //以下语句用于统计仿真总用时，
  Time txBegin = Now();
  Simulator::Run ();
  Time txLast = Now() - txBegin;
  cout<<txLast.GetSeconds()<<endl;

  ////打开Sqlite数据库或omnet文件，写入统计数据
  ////打开Sqlite数据库或omnet文件，写入统计数据
  ////打开Sqlite数据库或omnet文件，写入统计数据
  Ptr<DataOutputInterface> output = 0;
  if (format == "omnet") {
      NS_LOG_INFO ("Creating omnet formatted data output.");
      output = CreateObject<OmnetDataOutput>();
    } else if (format == "db") {
    #ifdef STATS_HAS_SQLITE3
      NS_LOG_INFO ("Creating sqlite formatted data output.");
      output = CreateObject<SqliteDataOutput>();
      /////下面这句具体修改sqlite3文件名称，扩展名为db，如下，则文件为：mydata.db，没有指明目录，则在ns-allinone-3.29/ns-3.29目录下
      output->SetFilePrefix("mydata");
    #endif
    } else {
      NS_LOG_ERROR ("Unknown output format " << format);
    }

  if (output != 0)
    output->Output (data);
  ////打开Sqlite数据库或omnet文件，写入统计数据
  ////打开Sqlite数据库或omnet文件，写入统计数据
  ////打开Sqlite数据库或omnet文件，写入统计数据


  //释放仿真环境
  Simulator::Destroy ();
  return 0;
}
