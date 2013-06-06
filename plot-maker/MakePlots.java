import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Set;


//To plot scattered in gnuplot, say -> plot 'filename'
//To plot unscattered, say -> plot 'filename' with lines

public class PlotMaker {
  
	static HashMap<Integer, SSDObject> SSDMap;
	static HashMap<Integer, RamObject> RamMap;
	static int traceLength;
	static long [] v;
	
	
	/**
	 *  This program accepts three files: trace miss curve, Solid State Disk specs
	 *and Ram specs; it outputs files that gives the cost/delay and cost/capacity combo for every 
	 *possible SSD and Ram
	 * 
	 * @param args: Trace file, SSD file, Ram file
	 * @throws IOException
	 */
	public static void main (String[] args) throws IOException {
		if (args.length !=3) {
			
		    System.err.println("Usage: java main " +
				       "<traceName>, " + //first argument is the name of the traceMissCurve file
				       " < File of SSDInfo>, <File of RamInfo>"); 
		    System.exit(1);
		}
		
		traceLength=deriveTraceLength(args[0]);
			
		//We create a map of RamObjects and SSDObjects. The key of the map is 
		//their capacity, the object is the RamObject or SSD Object (depending on the map)
		makeMaps(args[1], args[2]);
		//Using the information from the maps as well as the Trace file, we create files
		//that gives the delays and costs of every SSD/Ram combination
		makePlot(args[0]);
		}
		
/*
 * Derives the trace length
 * */
	private static int deriveTraceLength(String string) throws FileNotFoundException {
		Scanner s=new Scanner(new File(string));
		int num=0; String st;
		while (s.hasNext()){
			st=s.next();
			st=s.next();
			num++;	
		}
			return num;
	}


	/**
	 * 
	 * This method assumes an existence of a files for HDD and SSD with the 
	 * list of capacities, costs and speeds. It creates a map of RamObjects and SSDObjects.
	 * Key of the maps is the memory capacity of Ram/SSD
	 * 
	 * @param f1: SSD file name
	 * @param f2 Ram file name
	 * @throws FileNotFoundException
	 */
	public static void makeMaps (String f1, String f2) throws FileNotFoundException{
		
		//Creating RamMap 
		Scanner sc=new Scanner (new File(f2));
		int capacity; double cost; double speed;
		RamMap=new HashMap<Integer, RamObject>();
		
		while (sc.hasNext()){
			capacity=sc.nextInt(); 
			cost=sc.nextDouble();
			speed=sc.nextDouble();
			RamObject a=new RamObject(capacity, cost, speed);
			RamMap.put(capacity, a);
			
		}
		sc.close();
		//Creating SSDMap 
				Scanner sc2=new Scanner (new File(f1));
				SSDMap=new HashMap<Integer, SSDObject>();
				capacity=0;  cost=0.0; speed=0.0; //reset
				
				while (sc2.hasNext()){
					capacity=sc2.nextInt(); 
					cost=sc2.nextDouble();
					speed=sc2.nextDouble();
					SSDObject b=new SSDObject(capacity, cost, speed);
					SSDMap.put(capacity, b);
					
				}
		sc2.close();
	}
	
	/**
	 * Using the information from the maps as well as the Trace file, we create 2 files
	 * for scattered plots that give the cost/delay and cost/capacity of every SSD/Ram 
	 * combination. Additionally, we get files with the same information for each of the 
	 * unscattered plots with for each constant ram sizes
	 * 
	 * @param trace: Trace file name
	 * @throws IOException
	 */
	public static void makePlot(String trace) throws IOException{
		String output="scatteredCostDelay";
		String out2="scatteredCostCapacity";
		double delay=0.0; long capacity;
		double costR=0.0; double costS=0.0; double cost=0.0;
		int Rs=0; int Ss=0;
		String s="";
		//get KeySet from RamMap and SSDMap; the keySet is capacities, so we'd run
		//that through for-loops by an iterator
		Set<Integer> RsSet=RamMap.keySet();
		Iterator<Integer> RsI=RsSet.iterator();
		putInArray(trace);
		FileWriter fw=new FileWriter (output);
		FileWriter fw2=new FileWriter (out2);
		int traceMax= traceLength/256;
		
		while ( RsI.hasNext()){
			Rs=RsI.next();
			Set<Integer> SsSet=SSDMap.keySet();
			Iterator<Integer> SsI=SsSet.iterator();
			
			int augF=(traceMax-Rs)/50;
			
			while( SsI.hasNext()){ 
				Ss=SsI.next();
				
				int augS=(traceMax-Ss)/50;
				
				double St=getSt(Ss); //SSD time
				double Ht=getHt(); //HDD time
				costR=RamMap.get(Rs).getCost();
				costS=SSDMap.get(Ss).getCost();
				int p=0;
				for (int i=Rs; i<traceMax+1; i=i+augF){
					p++;
					String plot="ramConstant"+i+"Unscattered";
					plot=plot+p;
					FileWriter unsc1=new FileWriter (plot+"_1");
					FileWriter unsc2=new FileWriter (plot+"_2");
					for (int j=i; j<traceMax+1; j=j+augS){
						long MSs=getMs(trace, j); //Misses to SSD's
						long MRs=getMs(trace, i); //Misses to RAM
						delay=(Ht*MSs)+(St*MRs); //+(StxMRs) for later
						capacity =i+j;
						cost=(((double)i)/(double)Rs)*costR +(((double)j)/(double)Ss)*costS;
						//Scattered plot for Cost/Delay
						s=""+cost+" "+delay;
						fw.write(s);
						fw.write("\n");
						fw.flush();
						//Unscattered plot for Cost/delay
						unsc1.write(s);
						unsc1.write("\n");
						unsc1.flush();
					
						//Scattered plot for Cost/Capacity
						s=""+cost+" "+capacity;
						fw2.write(s);
						fw2.write("\n");
						fw2.flush();
						
						//Scattered plot for Cost/Capacity
						unsc2.write(s);
						unsc2.write("\n");
						unsc2.flush();
					
					}
					unsc1.close();
					unsc2.close();
					}
			
			}
		}
		fw.close();
		fw2.close();
		
	}

	/**
	 * creates an array of trace with index being the capacity and object being the
	 * number of misses
	 * 
	 * @param trace: name of the file w/ the trace
	 * @throws FileNotFoundException
	 */
	private static void putInArray(String trace) throws FileNotFoundException {
		v=new long[traceLength+1]; 
			
		Scanner sc=new Scanner (new File(trace));
		int a=0; 
		int b = 0; //if at the end, b==0, then scanner had no strings at all.
		while (sc.hasNext()){
			
			a = sc.nextInt();
			b = sc.nextInt();

			v[a]=b;
				
			
				
		}
			sc.close();
		
	}


	/**
	 * Goes to the trace file and looks up the number of misses for that Memory Size
	 * 
	 * @param trace: traceFile
	 * @param MSize: the size of the Ram or SSD
	 * @return: number of misses
	 * @throws FileNotFoundException
	 */
	private static long getMs(String trace, int MSize) throws FileNotFoundException {
		
		MSize=(MSize*1024)/4;
	return v[MSize];
	}
	
	

	/**
	 * Gets the speed of the SSD of a particular size
	 * 
	 * @param SSDSize: SSD capacity
	 * @return: speed of that SSD
	 */
	private static double getSt(int SSDSize) {
		
		
		SSDObject sd=SSDMap.get(SSDSize);
		
		return sd.getSpeed();

	}


	/**
	 * @return the speed of the fixed HDD we're assuming to have
	 */
	private static double getHt() {
		//we have picked a fixed HD with a fixed time, capacity and speed.
		double HardDriveTime=2000000; //2 =seekTime+rmp in ms. multiplied by a million
		return HardDriveTime;
	}
		
}
