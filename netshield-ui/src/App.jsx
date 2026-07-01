import React, { useState, useEffect, useRef } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';
import { Activity, AlertTriangle, ShieldCheck, Server, Cpu, ShieldAlert, PlayCircle } from 'lucide-react';

const App = () => {
  const [stats, setStats] = useState({ total_packets: 0, total_alerts: 0, active_ips: [] });
  const [aiAlerts, setAiAlerts] = useState([]);
  const [trafficHistory, setTrafficHistory] = useState([]);
  
  // Toggle for Resume/Portfolio showcase
  const [isDemoMode, setIsDemoMode] = useState(true); 
  
  const prevPacketCount = useRef(0);

  useEffect(() => {
    // Reset dashboard on mode switch
    setStats({ total_packets: 0, total_alerts: 0, active_ips: [] });
    setAiAlerts([]);
    setTrafficHistory([]);
    prevPacketCount.current = 0;

    let interval;

    if (isDemoMode) {
      // ==========================================
      // 🚀 DEMO MODE: SIMULATES BACKEND DATA
      // ==========================================
      interval = setInterval(() => {
        const pps = Math.floor(Math.random() * 150) + 50; // Random traffic 50-200 pps
        
        setStats(prev => {
          const newTotal = prev.total_packets + pps;
          return {
            total_packets: newTotal,
            total_alerts: prev.total_alerts,
            active_ips: [
              {"172.28.71.211": Math.floor(newTotal * 0.4)},
              {"167.82.56.223": Math.floor(newTotal * 0.2)},
              {"151.101.0.223": Math.floor(newTotal * 0.15)},
              {"185.125.190.57": Math.floor(newTotal * 0.1)},
              {"151.101.128.223": Math.floor(newTotal * 0.05)}
            ]
          };
        });

        // Simulate random AI anomalies (approx 5% chance per second)
        if (Math.random() < 0.05) {
          const fakeIps = ["192.168.1.105", "10.0.0.55", "172.16.0.42"];
          const randomIp = fakeIps[Math.floor(Math.random() * fakeIps.length)];
          
          setAiAlerts(prev => {
            const newAlerts = [{
              timestamp: new Date().toLocaleTimeString('en-US', { hour12: false }),
              ip: randomIp,
              pkts: Math.floor(Math.random() * 500) + 200,
              ports: Math.floor(Math.random() * 150) + 50
            }, ...prev];
            return newAlerts.slice(0, 50); // Keep last 50 alerts
          });
        }

        const now = new Date().toLocaleTimeString('en-US', { hour12: false, hour: "numeric", minute: "numeric", second: "numeric" });
        setTrafficHistory(prev => {
          const newHistory = [...prev, { time: now, pps: pps }];
          return newHistory.length > 20 ? newHistory.slice(1) : newHistory;
        });
      }, 1000);

    } else {
      // ==========================================
      // 📡 LIVE MODE: FETCHES FROM REAL C++/PYTHON
      // ==========================================
      const fetchData = async () => {
        try {
          const statsRes = await fetch('http://localhost:8080/stats');
          const statsData = await statsRes.json();
          setStats(statsData);

          const aiRes = await fetch('http://localhost:8081/api/alerts');
          const aiData = await aiRes.json();
          setAiAlerts(aiData);

          const pps = statsData.total_packets - prevPacketCount.current;
          prevPacketCount.current = statsData.total_packets;

          const now = new Date().toLocaleTimeString('en-US', { hour12: false, hour: "numeric", minute: "numeric", second: "numeric" });
          setTrafficHistory(prev => {
            const newHistory = [...prev, { time: now, pps: pps > 0 ? pps : 0 }];
            return newHistory.length > 20 ? newHistory.slice(1) : newHistory;
          });
        } catch (error) {
          console.error("Waiting for real backend connections...");
        }
      };

      fetchData();
      interval = setInterval(fetchData, 1000);
    }
    
    return () => clearInterval(interval);
  }, [isDemoMode]); 

  const formatActiveIps = (ipsArray) => {
    return ipsArray.map(obj => {
      const ip = Object.keys(obj)[0];
      return { ip, count: obj[ip] };
    }).sort((a, b) => b.count - a.count);
  };

  return (
    <div className="min-h-screen p-6 font-sans bg-slate-900 text-slate-100">
      
      {/* Header */}
      <header className="flex items-center justify-between mb-8">
        <div>
          <h1 className="text-3xl font-bold text-emerald-400 tracking-tight flex items-center gap-3">
            <ShieldCheck size={32} /> NetShield SOC
          </h1>
          <p className="text-slate-400 mt-1">Dual-Engine Packet Inspection & ML Anomaly Detection</p>
        </div>
        
        {/* DEMO MODE TOGGLE BUTTON */}
        <button 
          onClick={() => setIsDemoMode(!isDemoMode)}
          className={`flex items-center gap-2 px-4 py-2 rounded-full border transition-all duration-300 hover:scale-105 ${
            isDemoMode 
            ? 'bg-amber-500/10 text-amber-400 border-amber-500/30 shadow-[0_0_15px_rgba(245,158,11,0.2)]' 
            : 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20 shadow-[0_0_15px_rgba(16,185,129,0.2)]'
          }`}
        >
          <span className="relative flex h-3 w-3">
            <span className={`animate-ping absolute inline-flex h-full w-full rounded-full opacity-75 ${isDemoMode ? 'bg-amber-400' : 'bg-emerald-400'}`}></span>
            <span className={`relative inline-flex rounded-full h-3 w-3 ${isDemoMode ? 'bg-amber-500' : 'bg-emerald-500'}`}></span>
          </span>
          {isDemoMode ? (
            <span className="flex items-center gap-2"><PlayCircle size={18} /> Demo Mode Active</span>
          ) : (
            'System Online (Live Data)'
          )}
        </button>
      </header>

      {/* Top Stat Cards */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-6 mb-8">
        <div className="p-6 bg-slate-800 rounded-xl border border-slate-700 shadow-lg flex items-center justify-between">
          <div>
            <p className="text-slate-400 text-sm font-medium mb-1">Total Packets Inspected</p>
            <h2 className="text-3xl font-bold text-white">{stats.total_packets.toLocaleString()}</h2>
          </div>
          <div className="p-3 bg-blue-500/10 rounded-lg text-blue-400"><Activity size={28} /></div>
        </div>

        <div className="p-6 bg-slate-800 rounded-xl border border-slate-700 shadow-lg flex items-center justify-between">
          <div>
            <p className="text-slate-400 text-sm font-medium mb-1">Active Endpoints</p>
            <h2 className="text-3xl font-bold text-white">{stats.active_ips.length}</h2>
          </div>
          <div className="p-3 bg-purple-500/10 rounded-lg text-purple-400"><Server size={28} /></div>
        </div>

        <div className="p-6 bg-slate-800 rounded-xl border border-slate-700 shadow-lg flex items-center justify-between">
          <div>
            <p className="text-slate-400 text-sm font-medium mb-1">IDS Static Blocks</p>
            <h2 className={`text-3xl font-bold ${stats.total_alerts > 0 ? 'text-amber-500' : 'text-slate-300'}`}>
              {stats.total_alerts}
            </h2>
          </div>
          <div className="p-3 bg-amber-500/10 rounded-lg text-amber-500"><AlertTriangle size={28} /></div>
        </div>

        <div className="p-6 bg-slate-800 rounded-xl border border-rose-500/30 shadow-[0_0_15px_rgba(244,63,94,0.1)] flex items-center justify-between">
          <div>
            <p className="text-rose-400 text-sm font-medium mb-1">AI Anomalies</p>
            <h2 className={`text-3xl font-bold ${aiAlerts.length > 0 ? 'text-rose-500' : 'text-slate-300'}`}>
              {aiAlerts.length}
            </h2>
          </div>
          <div className="p-3 bg-rose-500/10 rounded-lg text-rose-500"><Cpu size={28} /></div>
        </div>
      </div>

      {/* Main Content Layout */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-6">
        
        {/* Real-time Traffic Chart */}
        <div className="lg:col-span-2 p-6 bg-slate-800 rounded-xl border border-slate-700 shadow-lg">
          <h3 className="text-xl font-semibold mb-6 flex items-center gap-2">
            <Activity size={24} className="text-blue-400" /> Live Network Throughput (PPS)
          </h3>
          <div className="h-72 w-full">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={trafficHistory}>
                <CartesianGrid strokeDasharray="3 3" stroke="#334155" vertical={false} />
                <XAxis dataKey="time" stroke="#94a3b8" fontSize={12} tickMargin={10} />
                <YAxis stroke="#94a3b8" fontSize={12} tickFormatter={(val) => `${val} pkt`} />
                <Tooltip contentStyle={{ backgroundColor: '#1e293b', borderColor: '#334155', color: '#f8fafc' }} />
                <Line type="monotone" dataKey="pps" stroke="#38bdf8" strokeWidth={3} dot={false} activeDot={{ r: 6, fill: "#38bdf8", stroke: "#0f172a" }} animationDuration={300} />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Top IPs Table */}
        <div className="p-6 bg-slate-800 rounded-xl border border-slate-700 shadow-lg flex flex-col h-[380px]">
          <h3 className="text-xl font-semibold mb-4 flex items-center gap-2">
            <Server size={24} className="text-purple-400" /> Top Talkers
          </h3>
          <div className="overflow-auto flex-1 pr-2 custom-scrollbar">
            <table className="w-full text-left border-collapse">
              <thead>
                <tr className="border-b border-slate-700 text-slate-400 text-sm">
                  <th className="pb-3 font-medium">IP Address</th>
                  <th className="pb-3 font-medium text-right">Packets</th>
                </tr>
              </thead>
              <tbody>
                {formatActiveIps(stats.active_ips).map((endpoint, index) => (
                  <tr key={index} className="border-b border-slate-700/50 hover:bg-slate-700/30 transition-colors">
                    <td className="py-3 text-sm font-mono text-slate-300">{endpoint.ip}</td>
                    <td className="py-3 text-sm text-right text-emerald-400 font-medium">
                      {endpoint.count.toLocaleString()}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      </div>

      {/* AI Threat Intelligence Log */}
      <div className="p-6 bg-slate-900 rounded-xl border border-rose-900/50 shadow-[0_0_20px_rgba(244,63,94,0.05)]">
        <h3 className="text-xl font-semibold mb-4 flex items-center gap-2 text-rose-400">
          <ShieldAlert size={24} /> AI Threat Intelligence Log
        </h3>
        
        <div className="bg-black/50 rounded-lg p-4 font-mono text-sm border border-slate-800 h-64 overflow-y-auto custom-scrollbar">
          {aiAlerts.length === 0 ? (
            <div className="text-slate-500 h-full flex items-center justify-center">
              Scanning traffic patterns. No anomalies detected.
            </div>
          ) : (
            <div className="space-y-2">
              {aiAlerts.map((alert, idx) => (
                <div key={idx} className="flex items-center gap-4 text-rose-400/90 border-b border-rose-900/30 pb-2">
                  <span className="text-slate-500">[{alert.timestamp}]</span>
                  <span className="bg-rose-900/40 text-rose-400 px-2 py-0.5 rounded text-xs border border-rose-800 flex-shrink-0">OC-SVM ANOMALY</span>
                  <span className="flex-1 truncate">Suspicious variance detected from <b className="text-white">{alert.ip}</b></span>
                  <span className="text-slate-400 whitespace-nowrap">Vol: <span className="text-white">{alert.pkts} pkts</span></span>
                  <span className="text-slate-400 whitespace-nowrap">Ports: <span className="text-white">{alert.ports}</span></span>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

    </div>
  );
};

export default App;