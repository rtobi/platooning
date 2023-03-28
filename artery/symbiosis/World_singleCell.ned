//
// World_singleCell.ned
//
//      Author: rtobi
//
// world with 1 base station. Combines combine env.World and lte.World
//
package artery.symbiosis;

import inet.networklayer.ipv4.RoutingTableRecorder;

import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import lte.corenetwork.binder.LteBinder;
import lte.corenetwork.nodes.eNodeB;
import lte.corenetwork.nodes.Ue;
import lte.corenetwork.nodes.ExtCell;
import lte.epc.PgwStandardSimplified;
import lte.world.radio.LteChannelControl;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import artery.envmod.GlobalEnvironmentModel;
import artery.envmod.LocalEnvironmentModel;
import artery.inet.World;
import artery.utility.IdentityRegistry;

network World_singleCell extends artery.inet.World
{
    parameters:
        int numUe1 = default(0);
        int numUe2 = default(0);
        int numUe3 = default(0);
        int numExtCells = default(0);
        int numLteBaseStations = default(1);
        @display("i=block/network2;bgb=1701,858;bgi=background/budapest");
        traci.mapper.vehicleType = default("artery.symbiosis.Car");
        **.globalEnvironmentModule = default("environmentModel");
    submodules:
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=50,75;is=s");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=50,125");
        }
        binder: LteBinder {
            @display("p=50,175;is=s");
        }
        server: StandardHost {
            @display("p=173,48;is=n;i=device/server");
        }
        pgw: PgwStandardSimplified {
            nodeType = "PGW";
            @display("p=172,287;is=l");
        }
        router1: Router {
            @display("p=256,286;i=device/smallrouter");
        }
        eNodeB[numLteBaseStations]: eNodeB {
            parameters:
                mobility.initFromDisplayString = false;
                macNodeId = default(index + 1);
                macCellId = default(index + 1);
                **.eNodeBCount = default(numLteBaseStations);
        }
        ue1[numUe1]: Ue {
            @display("p=421,293");
        }
        ue2[numUe2]: Ue {
            @display("p=526,235");
        }
        ue3[numUe3]: Ue {
            @display("p=551,371");
        }
        extCell[numExtCells]: ExtCell {
            @display("p=100,480;is=vl");
        }
        environmentModel: GlobalEnvironmentModel {
            parameters:
                @display("p=50,225");
                identityRegistryModule = default("idRegistry");
                traciModule = default("traci");
                nodeMobilityModule = default(".mobility");
        }
        idRegistry: IdentityRegistry {
            parameters:
                @display("p=50,275");
        }
    connections:

        server.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> router1.pppg++;
        router1.pppg++ <--> Eth10G <--> eNodeB[0].ppp;
}
