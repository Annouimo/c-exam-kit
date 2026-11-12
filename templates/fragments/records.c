/* TODO: 按题意修改字段、排序规则与读入格式；独立示例，不自动接入其他组件 */
typedef struct { int id; long long score; } ExamRecord;
static ExamRecord records[ARRAY_MAX];
int record_count;
scanf("%d", &record_count);
for (int i = 0; i < record_count; i++) scanf("%d%lld", &records[i].id, &records[i].score);
/* TODO: 需要排序时自行提供比较器后qsort；按题意查询或修改records[i] */
