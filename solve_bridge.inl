#define GO(Reporter) \
    pfn(buffer_report_delayed, obj_goodness, Reporter) \
    pfn(buffer_report_immediate, obj_goodness, Reporter) \
    fn(obj_goodness, Reporter)

GO(debug_reporter)
GO(csv_reporter)
GO(cout_reporter)

#undef GO
// vim: set sw=4 sts=4 ts=8 noet:
