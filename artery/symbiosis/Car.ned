//
// Car.ned
//
//      Author: rtobi
//
//comibines env.Car and lte.Car

package artery.symbiosis;

import inet.applications.contract.ITCPApp;
import inet.applications.contract.IUDPApp;
import inet.networklayer.configurator.ipv4.HostAutoConfigurator;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import inet.transportlayer.tcp.TCP;
import inet.transportlayer.udp.UDP;
import lte.stack.ILteNic;
import artery.envmod.LocalEnvironmentModel;
import artery.inet.Car;

module Car extends artery.inet.Car
{
    parameters:
        // INET network layer
        *.routingTableModule = default(absPath(".routingTable"));

        // LTE NIC
        string nicType = default("LteNicUe");
        int macNodeId = default(0);
        int masterId;

        // application
        int numUdpApps = default(0);
        int numTcpApps = default(0);
        
        bool lteAvailable = default(true);

        @display("bgb=717,441");
    gates:
        input lteRadioIn @directIn;

    submodules:
        configurator: HostAutoConfigurator {
            @display("p=50,25");
        }

        routingTable: <default("IPv4RoutingTable")> like IRoutingTable {
            @display("p=650,261");
        }

        networkLayer: <default("IPv4NetworkLayer")> like INetworkLayer {
            @display("p=474,125");
        }

        lteNic: <nicType> like ILteNic {
            @display("p=474,400");
            nodeType = "UE";
        }

        udpApp[numUdpApps]: <> like IUDPApp {
            @display("p=250,25,col,-100");
        }

        udp: UDP if numUdpApps > 0 {
            @display("p=482,25");
        }

        tcpApp[numTcpApps]: <> like ITCPApp {
            @display("p=674,25,col,-100");
        }

        tcp: TCP if numTcpApps > 0 {
            @display("p=674,125");
        }

        environmentModel: LocalEnvironmentModel {
            @display("p=393,276");
            middlewareModule = default(".middleware");
        }

    connections allowunconnected:
        lteNic.radioIn <-- lteRadioIn;

        networkLayer.ifOut++ --> lteNic.upperLayerIn;
        networkLayer.ifIn++ <-- lteNic.upperLayerOut;

        for i=0..numUdpApps-1 {
            udpApp[i].udpOut --> udp.appIn++;
            udpApp[i].udpIn <-- udp.appOut++;
        }

        udp.ipOut --> networkLayer.transportIn++ if numUdpApps > 0;
        udp.ipIn <-- networkLayer.transportOut++ if numUdpApps > 0;

        for i=0..numTcpApps-1 {
            tcpApp[i].tcpOut --> tcp.appIn++;
            tcpApp[i].tcpIn <-- tcp.appOut++;
        }

        tcp.ipOut --> networkLayer.transportIn++ if numTcpApps > 0;
        tcp.ipIn <-- networkLayer.transportout++ if numTcpApps > 0;
}
