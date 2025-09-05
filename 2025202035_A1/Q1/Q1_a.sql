WITH stage_data AS (
    SELECT
        sa.StudentID,
        sa.StageName,
        sa.ExamDateTime,
        LEAD(sa.StageName) OVER (PARTITION BY sa.StudentID ORDER BY sa.ExamDateTime) AS next_stage,
        LEAD(sa.ExamDateTime) OVER (PARTITION BY sa.StudentID ORDER BY sa.ExamDateTime) AS next_exam_datetime
    FROM student_admissions sa
),
stage_summary AS (
    SELECT
        StageName,
        COUNT(DISTINCT StudentID) AS started,
        COUNT(DISTINCT CASE WHEN next_stage IS NOT NULL THEN StudentID END) AS moved_to_next,
        COUNT(DISTINCT CASE WHEN next_stage IS NULL THEN StudentID END) AS dropped,
        ROUND(IFNULL(AVG(TIMESTAMPDIFF(SECOND, ExamDateTime, next_exam_datetime) / 3600), 0), 2) AS avg_turnaround_hours
    FROM stage_data
    GROUP BY StageName
)
SELECT *
FROM stage_summary
ORDER BY StageName;