import pandas as pd
import time
from sklearn.svm import OneClassSVM
from sklearn.preprocessing import StandardScaler
import warnings
import os
from flask import Flask, jsonify
from flask_cors import CORS
import threading
from datetime import datetime

warnings.filterwarnings('ignore')

# ==========================================
# Flask API Setup (Runs on Port 8081)
# ==========================================
app = Flask(__name__)
CORS(app)
ai_alert_logs = []

@app.route('/api/alerts')
def get_alerts():
    # Return the latest 20 alerts, newest first
    return jsonify(ai_alert_logs[::-1][:20])

def start_api():
    # Suppress Flask console output to keep our terminal clean
    import logging
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)
    app.run(host='0.0.0.0', port=8081, debug=False, use_reloader=False)

# Launch API in the background
threading.Thread(target=start_api, daemon=True).start()

# ==========================================
# ML Pipeline 
# ==========================================
CSV_FILE = "build/traffic_features.csv"
print("=========================================")
print(" NetShield: AI Anomaly Detector (OC-SVM)")
print("=========================================")

model = OneClassSVM(nu=0.01, kernel="rbf", gamma="auto") 
scaler = StandardScaler()

last_processed_row = 0
is_trained = False

while True:
    if os.path.exists(CSV_FILE):
        try:
            # Production Fix 1: Read only what we need, or drop old data to save RAM
            df = pd.read_csv(CSV_FILE)
            
            df['total_packets'] = pd.to_numeric(df['total_packets'], errors='coerce')
            df['syn_count'] = pd.to_numeric(df['syn_count'], errors='coerce')
            df['unique_ports'] = pd.to_numeric(df['unique_ports'], errors='coerce')
            df = df.dropna() 
            
            # Production Fix 2: The Rolling Window (Keep only last 1000 points to prevent Memory Leaks)
            MAX_WINDOW_SIZE = 1000
            if len(df) > MAX_WINDOW_SIZE:
                df = df.tail(MAX_WINDOW_SIZE).reset_index(drop=True)
                # Adjust last_processed_row if the dataframe was truncated
                if last_processed_row > MAX_WINDOW_SIZE:
                    last_processed_row = MAX_WINDOW_SIZE

            if len(df) < 20: 
                print(f"\r[Status] Gathering baseline traffic... ({len(df)}/20 data points)", end="")
                time.sleep(2)
                continue
                
            raw_features = df[['total_packets', 'syn_count', 'unique_ports']]
            
            if not is_trained:
                print("\n\n[+] Sufficient baseline data collected.")
                print("[+] Scaling features and training One-Class SVM...")
                scaled_features = scaler.fit_transform(raw_features)
                model.fit(scaled_features)
                is_trained = True
                last_processed_row = len(df)
                print("[+] AI Model Active. API live on port 8081. Monitoring for Zero-Day attacks...\n")
                
            if is_trained and len(df) > last_processed_row:
                new_raw_data = raw_features.iloc[last_processed_row:]
                new_scaled_data = scaler.transform(new_raw_data)
                predictions = model.predict(new_scaled_data)
                
                for idx, pred in enumerate(predictions):
                    row_idx = last_processed_row + idx
                    ip = df.iloc[row_idx]['ip']
                    pkts = int(df.iloc[row_idx]['total_packets'])
                    ports = int(df.iloc[row_idx]['unique_ports'])
                    
                    if pred == -1:
                        print(f"🚨 [AI ALERT] Anomaly Detected! IP: {ip} | Pkts: {pkts} | Ports: {ports}")
                        ai_alert_logs.append({
                            "timestamp": datetime.now().strftime("%H:%M:%S"),
                            "ip": ip,
                            "pkts": pkts,
                            "ports": ports
                        })
                    else:
                        print(f"✅ [Normal] Traffic from IP: {ip} | Pkts: {pkts}")
                
                # Production Fix 3: Dynamic Retraining on the rolling window
                if len(df) % 50 == 0:
                    scaled_features = scaler.fit_transform(raw_features)
                    model.fit(scaled_features)
                    
                last_processed_row = len(df)
                
        except Exception as e:
            print(f"\n[Error during ML Pipeline] {e}")
            time.sleep(2)
            
    time.sleep(2)