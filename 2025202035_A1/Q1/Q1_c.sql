DELIMITER $$

CREATE PROCEDURE GetStudentStageSummary(IN in_student_id VARCHAR(20))
BEGIN
    -- Define the fixed list of stages
    WITH stages AS (
        SELECT 'Technical Entrance Test' AS StageName UNION ALL
        SELECT 'IQ Test' UNION ALL
        SELECT 'Descriptive Exam' UNION ALL
        SELECT 'Face-to-Face Interview'
    ),
    student_info AS (
        SELECT 
            sa.StudentID,
            CONCAT(sa.FirstName, ' ', sa.LastName) AS FullName,
            sa.Age,
            sa.City,
            sa.StageName,
            sa.Result,
            sa.Gender,
            sa.ExamDateTime,
            CASE 
                WHEN sa.Age BETWEEN 18 AND 20 THEN '18-20'
                WHEN sa.Age BETWEEN 21 AND 23 THEN '21-23'
                WHEN sa.Age BETWEEN 24 AND 25 THEN '24-25'
                ELSE 'Other'
            END AS age_band
        FROM student_admissions sa
        WHERE sa.StudentID = in_student_id
    )
    SELECT 
        in_student_id AS StudentID,
        COALESCE(si.FullName, 'NA') AS FullName,
        COALESCE(CAST(si.Age AS CHAR), 'NA') AS Age,
        COALESCE(si.City, 'NA') AS City,
        s.StageName,
        COALESCE(si.Result, 'NA') AS student_result,
        COALESCE(DATE_FORMAT(si.ExamDateTime, '%d-%m-%Y %H:%i'), 'NA') AS ExamDateTime,

        -- Gender-based comparison
        COALESCE(si.Gender, 'NA') AS Gender,
        ROUND(AVG(CASE WHEN sa.Result = 'Pass' THEN 1 ELSE 0 END), 2) AS gender_pass_rate,

        -- City-based comparison
        ROUND(AVG(CASE WHEN sa.Result = 'Pass' THEN 1 ELSE 0 END), 2) AS city_pass_rate,

        -- Age-band comparison
        COALESCE(si.age_band, 'NA') AS age_band,
        ROUND(AVG(CASE WHEN sa.Result = 'Pass' THEN 1 ELSE 0 END), 2) AS age_band_pass_rate

    FROM stages s
    LEFT JOIN student_info si 
        ON s.StageName = si.StageName
    LEFT JOIN student_admissions sa
        ON s.StageName = sa.StageName
       AND (si.City IS NULL OR sa.City = si.City)   -- filter peers by city
       AND sa.StudentID <> in_student_id

    GROUP BY s.StageName, si.FullName, si.Age, si.City, si.Result, si.Gender, si.age_band, si.ExamDateTime
    ORDER BY FIELD(s.StageName, 'Technical Entrance Test', 'IQ Test', 'Descriptive Exam', 'Face-to-Face Interview');
END$$

DELIMITER ;
