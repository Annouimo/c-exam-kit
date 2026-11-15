@COMMENT:records_0@
typedef struct { int id; long long score; } ExamRecord;
static ExamRecord records[ARRAY_MAX];
int record_count;
scanf("%d", &record_count);
for (int i = 0; i < record_count; i++) scanf("%d%lld", &records[i].id, &records[i].score);
@COMMENT:records_1@
