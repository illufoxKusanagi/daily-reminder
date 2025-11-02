"use client";

import { useEffect, useState } from "react";
import { CalendarBody } from "@/modules/calendar/calendar-body";
import { CalendarProvider } from "@/modules/calendar/contexts/calendar-context";
import { DndProvider } from "@/modules/calendar/contexts/dnd-context";
import { CalendarHeader } from "@/modules/calendar/header/calendar-header";
import { getEvents } from "@/modules/calendar/requests";
import type { IEvent } from "@/modules/calendar/interfaces";

export function Calendar() {
  const [events, setEvents] = useState<IEvent[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchEvents = async () => {
      try {
        setIsLoading(true);
        const data = await getEvents();
        console.log("Fetched events:", data);
        setEvents(data);
        setError(null);
      } catch (err) {
        console.error("Error loading events:", err);
        setError(err instanceof Error ? err.message : "Failed to load events");
      } finally {
        setIsLoading(false);
      }
    };

    fetchEvents();
  }, []);

  if (isLoading) {
    return (
      <div className="w-full border rounded-xl p-3 sm:p-6 lg:p-8">
        <div className="flex items-center justify-center min-h-[300px]">
          <div className="text-sm sm:text-base lg:text-lg">
            Loading calendar...
          </div>
        </div>
      </div>
    );
  }

  if (error) {
    return (
      <div className="w-full border rounded-xl p-3 sm:p-6 lg:p-8">
        <div className="flex flex-col items-center justify-center gap-4 min-h-[300px]">
          <div className="text-red-500 text-sm sm:text-base text-center">
            {error}
          </div>
          <button
            onClick={() => window.location.reload()}
            className="px-3 py-2 sm:px-4 sm:py-2 bg-primary text-primary-foreground rounded-md text-xs sm:text-sm"
          >
            Muat ulang
          </button>
        </div>
      </div>
    );
  }

  return (
    <CalendarProvider events={events} view="bulan">
      <DndProvider showConfirmation={false}>
        <div className="w-full h-full flex flex-col border rounded-xl overflow-hidden">
          <CalendarHeader />
          <div className="flex-1 overflow-auto">
            <CalendarBody />
          </div>
        </div>
      </DndProvider>
    </CalendarProvider>
  );
}
