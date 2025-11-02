"use client";

import type React from "react";
import { createContext, useContext, useState } from "react";
import { useLocalStorage } from "@/modules/calendar/hooks";
import type { IEvent, IUser } from "@/modules/calendar/interfaces";
import type { TCalendarView, TEventColor } from "@/modules/calendar/types";

interface ICalendarContext {
  selectedDate: Date;
  view: TCalendarView;
  setView: (view: TCalendarView) => void;
  agendaModeGroupBy: "date" | "color";
  setAgendaModeGroupBy: (groupBy: "date" | "color") => void;
  use24HourFormat: boolean;
  toggleTimeFormat: () => void;
  setSelectedDate: (date: Date | undefined) => void;
  selectedUserId: IUser["id"] | "all";
  setSelectedUserId: (userId: IUser["id"] | "all") => void;
  badgeVariant: "dot" | "colored";
  setBadgeVariant: (variant: "dot" | "colored") => void;
  selectedColors: TEventColor[];
  filterEventsBySelectedColors: (colors: TEventColor) => void;
  events: IEvent[];
  addEvent: (event: IEvent) => void;
  updateEvent: (event: IEvent) => void;
  removeEvent: (eventId: string) => void;
  clearFilter: () => void;
}

interface CalendarSettings {
  badgeVariant: "dot" | "colored";
  view: TCalendarView;
  use24HourFormat: boolean;
  agendaModeGroupBy: "date" | "color";
}

const DEFAULT_SETTINGS: CalendarSettings = {
  badgeVariant: "colored",
  view: "minggu",
  use24HourFormat: true,
  agendaModeGroupBy: "date",
};

const migrateViewValue = (view: string): TCalendarView => {
  const viewMigrationMap: Record<string, TCalendarView> = {
    day: "hari",
    week: "minggu",
    month: "bulan",
    year: "tahun",
    agenda: "agenda",
  };

  if (["hari", "minggu", "bulan", "tahun", "agenda"].includes(view)) {
    return view as TCalendarView;
  }

  if (viewMigrationMap[view]) {
    return viewMigrationMap[view];
  }

  console.warn(`Unknown view value: ${view}, falling back to minggu`);
  return "minggu";
};

const CalendarContext = createContext({} as ICalendarContext);

export function CalendarProvider({
  children,
  events,
  badge = "colored",
  view = "minggu",
}: {
  children: React.ReactNode;
  events: IEvent[];
  view?: TCalendarView;
  badge?: "dot" | "colored";
}) {
  const [settings, setSettings] = useLocalStorage<CalendarSettings>(
    "calendar-settings",
    {
      ...DEFAULT_SETTINGS,
      badgeVariant: badge,
      view: view,
    }
  );

  const migratedView = migrateViewValue(settings.view);
  if (migratedView !== settings.view) {
    console.log(`Migrating view from ${settings.view} to ${migratedView}`);
    setSettings({
      ...settings,
      view: migratedView,
    });
  }

  const [badgeVariant, setBadgeVariantState] = useState<"dot" | "colored">(
    settings.badgeVariant
  );
  const [currentView, setCurrentViewState] =
    useState<TCalendarView>(migratedView);
  const [use24HourFormat, setUse24HourFormatState] = useState<boolean>(
    settings.use24HourFormat
  );
  const [agendaModeGroupBy, setAgendaModeGroupByState] = useState<
    "date" | "color"
  >(settings.agendaModeGroupBy);

  const [selectedDate, setSelectedDate] = useState(new Date());
  const [selectedUserId, setSelectedUserId] = useState<IUser["id"] | "all">(
    "all"
  );
  const [selectedColors, setSelectedColors] = useState<TEventColor[]>([]);

  const [allEvents, setAllEvents] = useState<IEvent[]>(events || []);
  const [filteredEvents, setFilteredEvents] = useState<IEvent[]>(events || []);

  const updateSettings = (newPartialSettings: Partial<CalendarSettings>) => {
    setSettings({
      ...settings,
      ...newPartialSettings,
    });
  };

  const setBadgeVariant = (variant: "dot" | "colored") => {
    setBadgeVariantState(variant);
    updateSettings({ badgeVariant: variant });
  };

  const setView = (newView: TCalendarView) => {
    setCurrentViewState(newView);
    updateSettings({ view: newView });
  };

  const toggleTimeFormat = () => {
    const newValue = !use24HourFormat;
    setUse24HourFormatState(newValue);
    updateSettings({ use24HourFormat: newValue });
  };

  const setAgendaModeGroupBy = (groupBy: "date" | "color") => {
    setAgendaModeGroupByState(groupBy);
    updateSettings({ agendaModeGroupBy: groupBy });
  };

  const applyColorFilter = (
    eventsList: IEvent[],
    colors: TEventColor[] = selectedColors
  ) => {
    if (colors.length === 0) return eventsList;
    return eventsList.filter((event) => colors.includes(event.color || "blue"));
  };

  const filterEventsBySelectedColors = (color: TEventColor) => {
    const isColorSelected = selectedColors.includes(color);
    const newColors = isColorSelected
      ? selectedColors.filter((c) => c !== color)
      : [...selectedColors, color];

    setFilteredEvents(applyColorFilter(allEvents, newColors));
    setSelectedColors(newColors);
  };

  const handleSelectDate = (date: Date | undefined) => {
    if (!date) return;
    setSelectedDate(date);
  };

  const addEvent = (event: IEvent) => {
    const nextEvents = [...allEvents, event];
    setAllEvents(nextEvents);
    setFilteredEvents(applyColorFilter(nextEvents));
  };

  const updateEvent = (event: IEvent) => {
    const updated = {
      ...event,
      startDate: new Date(event.startDate).toISOString(),
      endDate: new Date(event.endDate).toISOString(),
    };

    const nextEvents = allEvents.map((e) => (e.id === event.id ? updated : e));
    setAllEvents(nextEvents);
    setFilteredEvents(applyColorFilter(nextEvents));
  };

  const removeEvent = (eventId: string) => {
    const nextEvents = allEvents.filter((e) => e.id !== eventId);
    setAllEvents(nextEvents);
    setFilteredEvents(applyColorFilter(nextEvents));
  };

  const clearFilter = () => {
    setFilteredEvents(allEvents);
    setSelectedColors([]);
    setSelectedUserId("all");
  };

  const value = {
    selectedDate,
    setSelectedDate: handleSelectDate,
    selectedUserId,
    setSelectedUserId,
    badgeVariant,
    setBadgeVariant,
    selectedColors,
    filterEventsBySelectedColors,
    events: filteredEvents,
    view: currentView,
    use24HourFormat,
    toggleTimeFormat,
    setView,
    agendaModeGroupBy,
    setAgendaModeGroupBy,
    addEvent,
    updateEvent,
    removeEvent,
    clearFilter,
  };

  return (
    <CalendarContext.Provider value={value}>
      {children}
    </CalendarContext.Provider>
  );
}

export function useCalendar(): ICalendarContext {
  const context = useContext(CalendarContext);
  if (!context)
    throw new Error("useCalendar must be used within a CalendarProvider.");
  return context;
}
