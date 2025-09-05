db.temperatures.aggregate([
  {
    $project: {
      city: 1,
      month: { $month: { $toDate: "$date" } },
      avg_c: "$temp.avg_c"
    }
  },
  {
    $group: {
      _id: { city: "$city", month: "$month", date: "$date" },
      daily_avg: { $avg: "$avg_c" }
    }
  },
  { $sort: { "_id.city": 1, "_id.date": 1 } }
])
