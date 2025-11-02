import { zodResolver } from "@hookform/resolvers/zod";
import { addMinutes, format, set } from "date-fns";
import { type ReactNode, useEffect, useMemo } from "react";
import { useForm } from "react-hook-form";
import { toast } from "sonner";
import { Button } from "@/components/ui/button";
import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "@/components/ui/form";
import { Input } from "@/components/ui/input";
import {
  Modal,
  ModalClose,
  ModalContent,
  ModalDescription,
  ModalFooter,
  ModalHeader,
  ModalTitle,
  ModalTrigger,
} from "@/components/ui/responsive-modal";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import { Textarea } from "@/components/ui/textarea";
import { Checkbox } from "@/components/ui/checkbox";
import { COLORS } from "@/modules/calendar/constants";
import { useCalendar } from "@/modules/calendar/contexts/calendar-context";
import { useDisclosure } from "@/modules/calendar/hooks";
import type { IEvent } from "@/modules/calendar/interfaces";
import { eventSchema, type TEventFormData } from "@/modules/calendar/schemas";
import { DialogClose } from "@radix-ui/react-dialog";
import { API_BASE_URL } from "@/types/backend-props";
import { DateTimePickerModern } from "@/components/ui/datetime-picker-modern";

interface IProps {
  children: ReactNode;
  startDate?: Date;
  startTime?: { hour: number; minute: number };
  event?: IEvent;
}

export function AddEditEventDialog({
  children,
  startDate,
  startTime,
  event,
}: IProps) {
  const { isOpen, onClose, onToggle } = useDisclosure();
  const { addEvent, updateEvent } = useCalendar();
  const isEditing = !!event;

  // Simple category list for personal daily activities
  const categories = [
    "Work",
    "Personal",
    "Health",
    "Shopping",
    "Exercise",
    "Study",
    "Social",
    "Home",
    "Other",
  ];

  const initialDates = useMemo(() => {
    if (!isEditing && !event) {
      if (!startDate) {
        const now = new Date();
        return { startDate: now, endDate: addMinutes(now, 30) };
      }
      const start = startTime
        ? set(new Date(startDate), {
            hours: startTime.hour,
            minutes: startTime.minute,
            seconds: 0,
          })
        : new Date(startDate);
      const end = addMinutes(start, 30);
      return { startDate: start, endDate: end };
    }

    return {
      startDate: new Date(event.startDate),
      endDate: new Date(event.endDate),
    };
  }, [startDate, startTime, event, isEditing]);

  const form = useForm<TEventFormData>({
    resolver: zodResolver(eventSchema),
    defaultValues: {
      title: event?.title || "",
      category: event?.category || "",
      description: event?.description || "",
      startDate: initialDates.startDate,
      endDate: initialDates.endDate,
      isReminderEnabled: event?.isReminderEnabled || false,
      color: event?.color || "blue",
      reminderTime: event?.reminderTime
        ? new Date(event.reminderTime)
        : undefined,
    },
  });

  useEffect(() => {
    form.reset({
      title: event?.title ?? "",
      category: event?.category ?? "",
      description: event?.description ?? "",
      startDate: initialDates.startDate,
      endDate: initialDates.endDate,
      color: event?.color ?? "blue",
      isReminderEnabled: event?.isReminderEnabled ?? false,
      reminderTime: event?.reminderTime
        ? new Date(event.reminderTime)
        : undefined,
    });
  }, [event, initialDates, form]);

  const onSubmit = async (values: TEventFormData) => {
    if (values.endDate <= values.startDate) {
      toast.error("End date must be after start date");
      return;
    }
    try {
      const payload = {
        title: values.title,
        category: values.category,
        description: values.description || "",
        startDate: format(values.startDate, "yyyy-MM-dd'T'HH:mm:ss"),
        endDate: format(values.endDate, "yyyy-MM-dd'T'HH:mm:ss"),
        color: values.color,
        reminderTime: values.reminderTime
          ? format(values.reminderTime, "yyyy-MM-dd'T'HH:mm:ss")
          : undefined,
        isReminderEnabled: values.isReminderEnabled ?? false,
      };

      if (isEditing) {
        const response = await fetch(`${API_BASE_URL}/api/event/${event.id}`, {
          method: "PUT",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(payload),
        });

        if (!response.ok) {
          throw new Error("Failed to update event");
        }

        const formattedEvent: IEvent = {
          id: event.id,
          ...payload,
        };
        updateEvent(formattedEvent);
        toast.success("Event updated successfully");
      } else {
        const response = await fetch(`${API_BASE_URL}/api/event`, {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(payload),
        });

        if (!response.ok) {
          const errorData = await response.json();
          throw new Error(errorData.error || "Failed to create event");
        }

        const result = await response.json();

        const formattedEvent: IEvent = {
          id: result.id,
          ...payload,
        };

        addEvent(formattedEvent);
        toast.success("Event created successfully");
      }

      onClose();
      form.reset();
    } catch (error) {
      console.error(`Error ${isEditing ? "editing" : "adding"} event:`, error);
      toast.error(`Failed to ${isEditing ? "edit" : "add"} event`);
    }
  };

  useEffect(() => {
    // No need to fetch config data anymore - using static category list
  }, []);

  return (
    <Modal open={isOpen} onOpenChange={onToggle} modal={false}>
      <ModalTrigger asChild>{children}</ModalTrigger>
      <ModalContent>
        <ModalHeader>
          <ModalTitle>{isEditing ? "Edit Event" : "Add New Event"}</ModalTitle>
          <ModalDescription>
            {isEditing
              ? "Modify your existing event."
              : "Create a new event for your calendar."}
          </ModalDescription>
        </ModalHeader>

        <Form {...form}>
          <form
            id="event-form"
            onSubmit={form.handleSubmit(onSubmit)}
            className="grid gap-4 py-4"
          >
            <FormField
              control={form.control}
              name="title"
              render={({ field, fieldState }) => (
                <FormItem>
                  <FormLabel htmlFor="title" className="required">
                    Title
                  </FormLabel>
                  <FormControl>
                    <Input
                      id="title"
                      placeholder="Enter a title"
                      {...field}
                      className={fieldState.invalid ? "border-red-500" : ""}
                    />
                  </FormControl>
                  <FormMessage />
                </FormItem>
              )}
            />
            <FormField
              control={form.control}
              name="category"
              render={({ field }) => (
                <FormItem>
                  <FormLabel htmlFor="category" className="required">
                    Category
                  </FormLabel>
                  <Select
                    onValueChange={field.onChange}
                    defaultValue={field.value as string}
                  >
                    <FormControl className="w-full">
                      <SelectTrigger>
                        <SelectValue placeholder="Select category" />
                      </SelectTrigger>
                    </FormControl>
                    <SelectContent>
                      {categories.map((category) => (
                        <SelectItem value={category} key={category}>
                          {category}
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                  <FormMessage />
                </FormItem>
              )}
            />
            <FormField
              control={form.control}
              name="startDate"
              render={({ field }) => (
                <FormItem>
                  <DateTimePickerModern
                    date={field.value}
                    setDate={field.onChange}
                    label="Start Date & Time"
                    placeholder="Select start date and time"
                  />
                  <FormMessage />
                </FormItem>
              )}
            />
            <FormField
              control={form.control}
              name="endDate"
              render={({ field }) => (
                <FormItem>
                  <DateTimePickerModern
                    date={field.value}
                    setDate={field.onChange}
                    label="End Date & Time"
                    placeholder="Select end date and time"
                  />
                  <FormMessage />
                </FormItem>
              )}
            />
            <FormField
              control={form.control}
              name="color"
              render={({ field, fieldState }) => (
                <FormItem>
                  <FormLabel className="required">Variant</FormLabel>
                  <FormControl>
                    <Select value={field.value} onValueChange={field.onChange}>
                      <SelectTrigger
                        className={`w-full ${
                          fieldState.invalid ? "border-red-500" : ""
                        }`}
                      >
                        <SelectValue placeholder="Select a variant" />
                      </SelectTrigger>
                      <SelectContent>
                        {COLORS.map((color) => (
                          <SelectItem value={color} key={color}>
                            <div className="flex items-center gap-2">
                              <div
                                className={`size-3.5 rounded-full bg-${color}-600 dark:bg-${color}-700`}
                              />
                              {color}
                            </div>
                          </SelectItem>
                        ))}
                      </SelectContent>
                    </Select>
                  </FormControl>
                  <FormMessage />
                </FormItem>
              )}
            />
            <FormField
              control={form.control}
              name="description"
              render={({ field, fieldState }) => (
                <FormItem>
                  <FormLabel>Description</FormLabel>
                  <FormControl>
                    <Textarea
                      {...field}
                      placeholder="Enter a description"
                      className={fieldState.invalid ? "border-red-500" : ""}
                    />
                  </FormControl>
                  <FormMessage />
                </FormItem>
              )}
            />

            {/* Reminder Section */}
            <div className="border-t pt-4 mt-2">
              <h3 className="text-sm font-medium mb-3">Reminder Settings</h3>

              <FormField
                control={form.control}
                name="isReminderEnabled"
                render={({ field }) => (
                  <FormItem className="flex flex-row items-start space-x-3 space-y-0 mb-4">
                    <FormControl>
                      <Checkbox
                        checked={field.value}
                        onCheckedChange={field.onChange}
                      />
                    </FormControl>
                    <div className="space-y-1 leading-none">
                      <FormLabel>Enable Reminder</FormLabel>
                      <p className="text-sm text-muted-foreground">
                        Get notified before this event starts
                      </p>
                    </div>
                  </FormItem>
                )}
              />

              {form.watch("isReminderEnabled") && (
                <FormField
                  control={form.control}
                  name="reminderTime"
                  render={({ field }) => (
                    <FormItem>
                      <DateTimePickerModern
                        date={field.value}
                        setDate={field.onChange}
                        label="Reminder Time"
                        placeholder="When to remind you"
                      />
                      <p className="text-xs text-muted-foreground mt-1">
                        Set when you want to be reminded about this event
                      </p>
                      <FormMessage />
                    </FormItem>
                  )}
                />
              )}
            </div>
          </form>
        </Form>
        <ModalFooter className="flex justify-end gap-2">
          <ModalClose asChild>
            <Button type="button" variant="outline">
              Cancel
            </Button>
          </ModalClose>
          <Button form="event-form" type="submit">
            {isEditing ? "Save Changes" : "Create Event"}
          </Button>
        </ModalFooter>
      </ModalContent>
      <DialogClose />
    </Modal>
  );
}
