"use client";

import { isSameDay, parseISO } from "date-fns";
import { motion } from "framer-motion";
import React from "react";
import { fadeIn, transition } from "@/modules/calendar/animations";
import { useCalendar } from "@/modules/calendar/contexts/calendar-context";
import { AgendaEvents } from "@/modules/calendar/views/agenda-view/agenda-events";
import { CalendarMonthView } from "@/modules/calendar/views/month-view/calendar-month-view";
import { CalendarWeekView } from "@/modules/calendar/views/week-and-day-view/calendar-week-view";
// import { CalendarDayView } from "@/modules/calendar/views/week-and-day-view/calendar-day-view";
import { CalendarYearView } from "@/modules/calendar/views/year-view/calendar-year-view";

export function CalendarBody() {
  const { view, events } = useCalendar();

  const singleDayEvents = events.filter((event) => {
    const startDate = parseISO(event.startDate);
    const endDate = parseISO(event.endDate);
    return isSameDay(startDate, endDate);
  });

  const multiDayEvents = events.filter((event) => {
    const startDate = parseISO(event.startDate);
    const endDate = parseISO(event.endDate);
    return !isSameDay(startDate, endDate);
  });

  return (
    <div className="w-full h-full overflow-scroll relative">
      <motion.div
        key={view}
        initial="initial"
        animate="animate"
        exit="exit"
        variants={fadeIn}
        transition={transition}
      >
        {view === "bulan" && (
          <CalendarMonthView
            singleDayEvents={singleDayEvents}
            multiDayEvents={multiDayEvents}
          />
        )}
        {view === "minggu" && (
          <CalendarWeekView
            singleDayEvents={singleDayEvents}
            multiDayEvents={multiDayEvents}
          />
        )}
        {/* {view === "hari" && (
          <CalendarDayView
            singleDayEvents={singleDayEvents}
            multiDayEvents={multiDayEvents}
          />
        )} */}
        {view === "tahun" && (
          <CalendarYearView
            singleDayEvents={singleDayEvents}
            multiDayEvents={multiDayEvents}
          />
        )}
        {view === "agenda" && (
          <motion.div
            key="agenda"
            initial="initial"
            animate="animate"
            exit="exit"
            variants={fadeIn}
            transition={transition}
          >
            <AgendaEvents />
          </motion.div>
        )}
      </motion.div>
    </div>
  );
}
