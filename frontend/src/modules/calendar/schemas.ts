import { z } from "zod";

export const eventSchema = z
  .object({
    title: z.string().min(1, "Title is required"),
    description: z.string().optional().or(z.literal("")),
    category: z.string().min(1, "Category is required"),
    startDate: z.date({
      message: "Start date is required",
    }),
    endDate: z.date({
      message: "End date is required",
    }),
    color: z.enum(["blue", "green", "red", "yellow", "purple", "orange"], {
      message: "Color is required",
    }),
    isReminderEnabled: z.boolean(),
    reminderTime: z.date().optional(),
  })
  .refine(
    (data) => {
      // If reminder is enabled, reminderTime must be set
      if (data.isReminderEnabled && !data.reminderTime) {
        return false;
      }
      return true;
    },
    {
      message: "Reminder time is required when reminder is enabled",
      path: ["reminderTime"],
    }
  );

export type TEventFormData = z.infer<typeof eventSchema>;
