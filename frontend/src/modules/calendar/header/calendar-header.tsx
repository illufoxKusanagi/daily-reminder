"use client";

import { motion } from "framer-motion";
import { Plus } from "lucide-react";

import { Button } from "@/components/ui/button";
import {
  slideFromLeft,
  slideFromRight,
  transition,
} from "@/modules/calendar/animations";
import { useCalendar } from "@/modules/calendar/contexts/calendar-context";
import { AddEditEventDialog } from "@/modules/calendar/dialogs/add-edit-event-dialog";
import { DateNavigator } from "@/modules/calendar/header/date-navigator";
import FilterEvents from "@/modules/calendar/header/filter";
import { TodayButton } from "@/modules/calendar/header/today-button";
import { Settings } from "@/modules/calendar/settings/settings";
import Views from "./view-tabs";

export function CalendarHeader() {
  const { view, events } = useCalendar();

  return (
    <div className="flex flex-col gap-2 sm:gap-3 lg:gap-4 border-b p-2 sm:p-3 lg:p-4 lg:flex-row lg:items-center lg:justify-between">
      <motion.div
        className="flex flex-row justify-between sm:justify-center items-stretch xs:items-center gap-2 sm:gap-10 m-2"
        variants={slideFromLeft}
        initial="initial"
        animate="animate"
        transition={transition}
      >
        <TodayButton />
        <DateNavigator view={view} events={events} />
      </motion.div>

      <motion.div
        className="flex flex-col gap-4 sm:gap-3 lg:flex-row lg:items-center lg:gap-4"
        variants={slideFromRight}
        initial="initial"
        animate="animate"
        transition={transition}
      >
        <div className="flex flex-col sm:flex-row items-stretch sm:justify-center sm:items-center gap-2">
          <FilterEvents className="hidden sm:inline" />
          <Views />
        </div>

        <div className="flex flex-row gap-2 justify-between sm:justify-start">
          <FilterEvents className="inline sm:hidden" />
          <AddEditEventDialog>
            <Button className="flex-1 xs:flex-none text-xs sm:text-sm">
              <Plus className="h-3 w-3 sm:h-4 sm:w-4 mr-1" />
              <span className="inline">Tambah Agenda</span>
            </Button>
          </AddEditEventDialog>
          <Settings />
        </div>
      </motion.div>
    </div>
  );
}
