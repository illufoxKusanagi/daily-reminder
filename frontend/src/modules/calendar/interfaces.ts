import type { TEventColor } from "@/modules/calendar/types";

export interface IUser {
  id: string;
  name: string;
  picturePath: string | null;
}

export interface IEvent {
  id: string;
  category: string; // Changed from opdName to category (Work, Personal, Health, etc.)
  startDate: string;
  endDate: string;
  title: string;
  color: TEventColor;
  description: string;
  reminderTime?: string; // Optional: When to show reminder (ISO datetime)
  isReminderEnabled?: boolean; // Whether reminder is active
}

export interface ICalendarCell {
  day: number;
  currentMonth: boolean;
  date: Date;
}
