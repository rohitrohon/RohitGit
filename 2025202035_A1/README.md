--------------------------------------------Q1---------------------------------------------------------------------------------------
# Student Admissions SQL Solutions  

## Problem Overview  
The task is to analyze student admissions data stored in a relational database (`student_admissions` table). The analysis is divided into three main parts:  

1. **Admission Funnel** – Provide a stage-by-stage representation of how applicants progress through the admission process, including turn-around time between stages.  
2. **Pass and Fail Rate** – Calculate pass rates segmented by gender, age band, and city, for each stage.  
3. **Student Stage Summary (Stored Procedure)** – Given a `StudentID`, summarize that student’s performance in each stage and compare it against peers by gender, city, and age.  

---

## Approach  

### **Q1: Admission Funnel**  
- **Objective:** Show how many students started, advanced to the next stage, or dropped out at each admission stage, along with the average turn-around time between stages.  
- **Method:**  
  - Used a **CTE (`stage_data`)** with `LEAD()` window function to identify the next stage and timestamp for each student.  
  - Summarized results per stage to calculate:  
    - **Started:** Count of unique students who reached the stage.  
    - **Moved to Next:** Count of students progressing further.  
    - **Dropped:** Count of students who did not progress.  
    - **Average Turnaround Hours:** Average time (in hours) between two consecutive stages.  

**Sample Output Structure:**  

| StageName    | Started | Moved_To_Next | Dropped | Avg_Turnaround_Hours |
|--------------|---------|---------------|---------|----------------------|
| Application  | 120     | 100           | 20      | 48.50                |
| Written Test | 100     | 75            | 25      | 36.25                |
| Interview    | 75      | 60            | 15      | 24.75                |
| Final        | 60      | 0             | 60      | 0.00                 |

---

### **Q2: Pass and Fail Rate Analysis**  
- **Objective:** Measure pass rates by gender, age band, and city at each stage.  
- **Method:**  
  - Grouped data by **StageName**, **Gender**, **Age Band**, and **City**.  
  - Used `CASE` logic to categorize ages into bands:  
    - `18-20`, `21-23`, `24-25`, `Other`.  
  - Calculated pass rates as:  
    \[
    \text{Pass Rate} = \frac{\text{Number of Passes}}{\text{Total Students}} \times 100
    \]  
  - Output includes breakdown by **Stage, Gender, Age Band, City**.  

**Sample Output Structure:**  

| StageName    | Gender | Pass_Rate_Gender | Age_Band | Pass_Rate_Age_Band | City      | Pass_Rate_City |
|--------------|--------|------------------|----------|--------------------|-----------|----------------|
| Application  | Male   | 78.50            | 18-20    | 72.30              | Delhi     | 80.00          |
| Application  | Female | 82.40            | 21-23    | 76.50              | Mumbai    | 78.20          |
| Written Test | Male   | 65.20            | 24-25    | 69.30              | Delhi     | 70.10          |
| Interview    | Female | 71.40            | Other    | 60.00              | Bangalore | 68.50          |

---

### **Q3: Stored Procedure for Student Stage Summary**  
- **Objective:** Provide a detailed summary for a given student across the stages they attempted, comparing their results with peers.  
- **Method:**  
  - Created procedure `GetStudentStageSummary(StudentID)`.  
  - Steps:  
    - Extract student info and categorize into **gender, city, age band**.  
    - Join with peers in the same stage (excluding the student themselves).  
    - Compute **peer group pass rates** by gender, city, and age band for context.  
  - Output includes: Student details, their result per stage, and the average performance of other students in the same dimension.  

**Sample Output Structure:**  

| FullName      | Age | City   | StageName    | Student_Result | Gender | Gender_Pass_Rate | City_Pass_Rate | Age_Band | Age_Band_Pass_Rate |
|---------------|-----|--------|--------------|----------------|--------|------------------|----------------|----------|--------------------|
| Rohit Sharma  | 22  | Delhi  | Application  | Pass           | Male   | 0.78             | 0.80           | 21-23    | 0.76               |
| Rohit Sharma  | 22  | Delhi  | Written Test | Fail           | Male   | 0.65             | 0.70           | 21-23    | 0.68               |
| Rohit Sharma  | 22  | Delhi  | Interview    | Pass           | Male   | 0.72             | 0.74           | 21-23    | 0.69               |

---

## Assumptions  

1. **Dataset:** All queries assume the dataset is loaded into a relational database with table name `student_admissions`.  
2. **Date Fields:** The `ExamDateTime` column is used to compute stage progression and turnaround time.  
3. **Unique Students:** Each `StudentID` uniquely identifies a student across stages.  
4. **Age Banding:** Fixed age groups (`18-20`, `21-23`, `24-25`, `Other`) are applied consistently.  
5. **Performance Metrics:**  
   - Pass rates are expressed as percentages in **Q2**.  
   - Peer comparisons in **Q3** are expressed as mean pass rates (values between 0 and 1).  

---

## Deliverables  

- **SQL Query 1:** Admission Funnel Analysis  
- **SQL Query 2:** Pass Rate Analysis by Gender, Age Band, and City  
- **SQL Query 3:** Stored Procedure for Student Stage Summary  
-----------------------------------------------------------------------------------------------------------------------------------


--------------------------------------------Q2---------------------------------------------------------------------------------------
# Climate Data Analysis (Jan–Jun 2025) – README

## 📂 Dataset  
The dataset contains **daily temperature and weather metrics** for 100 Indian cities, from **Jan–Jun 2025**, split into `.jsonl` files.  

- `temperatures_India_100cities_2025_*.jsonl` → Daily temperature records  
- `weather_India_100cities_2025_*.jsonl` → Daily weather conditions  

Each record represents **one city on one date**. 


⚙️ Database Setup
Create database and import data:
mongoimport --db climateDB --collection temperatures --file temperatures_India_100cities_2025_Jan.jsonl --jsonArray
mongoimport --db climateDB --collection weather --file weather_India_100cities_2025_Jan.jsonl --jsonArray

Indexes for performance:
db.temperatures.createIndex({ city: 1, date: 1 });
db.weather.createIndex({ city: 1, date: 1 });


🔎 Queries Implemented
(a) Temperature Analysis
## Daily average per city by month ## 
db.temperatures.aggregate([
  { $project: { city: 1, month: { $month: { $toDate: "$date" } }, avg_c: "$temp.avg_c" }},
  { $group: { _id: { city: "$city", month: "$month", date: "$date" }, daily_avg: { $avg: "$avg_c" }}}
]}
## Monthly average per city ##
db.temperatures.aggregate([
  { $project: { city: 1, month: { $month: { $toDate: "$date" } }, avg_c: "$temp.avg_c" }},
  { $group: { _id: { city: "$city", month: "$month" }, monthly_avg: { $avg: "$avg_c" }}}
])
## Hottest & coldest cities overall (Jan–Jun) ##
db.temperatures.aggregate([
  { $group: { _id: "$city", overall_avg: { $avg: "$temp.avg_c" }}},
  { $sort: { overall_avg: -1 }},
  { $facet: {
      hottest: [{ $limit: 1 }],
      coldest: [{ $sort: { overall_avg: 1 }}, { $limit: 1 }]
  }}
])
(b) Extended Analysis
## Top 5 hottest & coldest days nationwide ##
db.temperatures.aggregate([
  { $group: { _id: "$date", national_avg: { $avg: "$temp.avg_c" }}},
  { $facet: {
      hottest_days: [{ $sort: { national_avg: -1 }}, { $limit: 5 }],
      coldest_days: [{ $sort: { national_avg: 1 }}, { $limit: 5 }]
  }}
])
## Rain check for a city on a date ##
db.weather.find({ city: "Mumbai", date: "2025-06-15" }, { precip_mm: 1, condition: 1 })
Rain if precip_mm > 0 OR condition contains “Rain”/“Thunderstorm”.
## 7-day moving average for one city ##
db.temperatures.aggregate([
  { $match: { city: "Delhi" }},
  { $setWindowFields: {
      partitionBy: "$city",
      sortBy: { date: 1 },
      output: {
        moving_avg_7d: {
          $avg: "$temp.avg_c",
          window: { documents: [-6, 0] }
        }
      }
  }}
])
✅ Assumptions
Daily average temperature (temp.avg_c) is taken directly from dataset (not recalculated from min/max).
Rain definition: Either precipitation > 0 mm OR weather condition includes “Rain” / “Thunderstorm”.
Top hottest/coldest days are based on national average across all 100 cities.
Moving average uses a 7-day rolling window (previous 6 days + current day).
Data provided is synthetic and assumed complete (no missing days per city).
-------------------------------------------------------------------------------------------------------------------------------------------
