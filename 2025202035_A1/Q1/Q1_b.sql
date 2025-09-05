SELECT 
    StageName,

    -- Pass rate by Gender
    Gender,
    ROUND(SUM(CASE WHEN Result = 'Pass' THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS pass_rate_gender,

    -- Pass rate by Age Band
    CASE 
        WHEN Age BETWEEN 18 AND 20 THEN '18-20'
        WHEN Age BETWEEN 21 AND 23 THEN '21-23'
        WHEN Age BETWEEN 24 AND 25 THEN '24-25'
        ELSE 'Other'
    END AS age_band,
    ROUND(SUM(CASE WHEN Result = 'Pass' THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS pass_rate_age_band,

    -- Pass rate by City
    City,
    ROUND(SUM(CASE WHEN Result = 'Pass' THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS pass_rate_city

FROM student_admissions
GROUP BY StageName, Gender,
         CASE 
             WHEN Age BETWEEN 18 AND 20 THEN '18-20'
             WHEN Age BETWEEN 21 AND 23 THEN '21-23'
             WHEN Age BETWEEN 24 AND 25 THEN '24-25'
             ELSE 'Other'
         END,
         City
ORDER BY StageName, City, age_band, Gender;